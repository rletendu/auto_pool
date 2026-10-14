# Plan : antigel économe en mode hiver (cycles marche/arrêt + condition eau)

## Contexte
En mode hiver, dès que `frost_risk_update()` ([filter_control.cpp:137](src/filter_control.cpp#L137)) détecte un risque de gel, la pompe tourne **en continu** (`filter_on()` à l'entrée dans `FILTER_AUTO_ACTIVE_FROST`, [filter_control.cpp:284](src/filter_control.cpp#L284)) jusqu'à ce que l'extérieur remonte à `frost_threshold` + 1,5 °C. Avec des gels fréquents, la consommation est trop élevée. Le mode REG (puissance réduite) n'est pas utilisable, donc la réduction passe par le temps de marche.

Objectif : garder la protection, mais faire tourner la pompe moins longtemps, en combinant :
- **D** : le gel n'est déclaré que si l'extérieur **et** l'eau sont froids ;
- **C** : cycles marche/arrêt pendant l'antigel, plus longs en heures creuses.

## Correction de ce que j'ai dit plus tôt
Le code de [measures.cpp:109-135](src/measures.cpp#L109) montre que `measures.water_temperature` n'est mise à jour que pompe en marche depuis plus de 30 s, et seulement si la lecture dépasse 1 °C. Pompe arrêtée, elle reste **figée** sur la dernière valeur. Deux conséquences :
- le repli actuel sur `water_temperature` peut être trompé : une valeur de 8 °C datant de la veille masque une canalisation à 1 °C ;
- ce que j'ai écrit sur « l'eau stagnante lue pompe arrêtée » était faux pour cette variable.

`measures.water_temperature_raw` est en revanche relue **toutes les 4 s**, pompe arrêtée comprise, et vaut 255 en cas d'erreur de sonde ([board.cpp:186](src/board.cpp#L186)). Le plan utilise donc `water_temperature_raw` pour la condition D. Si la sonde est dans la canalisation, comme le suggère le filtrage de `water_temperature`, c'est la température qui compte pour le gel.

## Comportement cible (mode hiver, filtre AUTO)

**Gel déclaré / levé**, évalué toutes les 10 s avec hystérésis :
| Situation | Gel déclaré | Gel levé |
|---|---|---|
| Extérieur reçu (< 30 min) | extérieur ≤ `frost_threshold` **et** eau froide | extérieur ≥ `frost_threshold` + 1,5 **ou** eau chaude |
| Pas d'extérieur | eau froide | eau chaude |

- Eau froide : `water_temperature_raw` ≤ **3 °C**. Eau chaude : ≥ **4 °C**.
- **Sonde en erreur** (raw ≥ 100) : l'eau est considérée comme inconnue, donc « froide » et jamais « chaude ». Sans extérieur ni sonde, le gel est déclaré par sécurité.

**Pompe pendant le gel** :
- **Gel sévère** (extérieur reçu et ≤ **-6 °C**, avec la condition eau toujours exigée) : marche continue.
- Sinon : cycles de 60 min. Marche **15 min** en heures pleines, **30 min** en heures creuses (**23 h → 7 h**). La durée de marche est choisie au début de chaque phase de marche.
- Le cycle démarre par une phase de marche à l'entrée dans l'état antigel.
- Si l'heure courante est dans le programme (`timer_prog_ok`), la pompe reste allumée même pendant la phase d'arrêt du cycle.
- Les règles existantes sont inchangées : OFF manuel prioritaire (alerte seulement), sortie du mode AUTO, sortie de l'état antigel.

## Modifications

1. **[src/config.h](src/config.h)** : `FROST_WATER_ON_C` passe de 4,0 à 3,0 et `FROST_WATER_OFF_C` de 5,0 à 4,0. Nouvelles constantes, entre parenthèses (le fichier contient déjà des `30 * 60` sans parenthèses) :
   `FROST_SEVERE_OUTDOOR_C` (-6,0f), `FROST_CYCLE_PERIOD_MIN` (60), `FROST_CYCLE_ON_MIN` (15), `FROST_CYCLE_ON_MIN_OFFPEAK` (30), `OFFPEAK_START_HOUR` (23), `OFFPEAK_END_HOUR` (7).

2. **[src/filter_control.cpp](src/filter_control.cpp)**
   - `static bool is_offpeak(uint8_t hour)` : gère la plage qui chevauche minuit (`start > end`). Elle utilise `rtc_get_hour()`, déjà employé dans ce fichier.
   - Réécrire `frost_risk_update()` selon le tableau ci-dessus. Elle garde son `static bool risk` et l'hystérésis. Elle expose en plus si le gel est sévère (par exemple via un second paramètre de sortie).
   - Dans `filter_control_update()`, au `case FILTER_AUTO_ACTIVE_FROST` : à la place de « pompe allumée en continu », piloter la pompe avec `want_on = timer_prog_ok || severe || phase_marche`. Compteurs `static` en ticks de `FILTER_CONTROL_UPDATE_S`, comme `counter_warm_up` et `counter_active_periodic`. On appelle `filter_on()` ou `filter_off()` ; les deux ne publient l'état MQTT qu'en cas de changement.
   - À l'entrée dans l'état antigel (bloc existant [filter_control.cpp:284](src/filter_control.cpp#L284)), initialiser la phase de marche et son compteur. Ajouter un `log_append` au passage en gel sévère et à la sortie.
   - Réutiliser telles quelles `filter_on()`, `filter_off()`, `log_append()`, `mqtt_publish_filter_state()`, `FILTER_CONTROL_UPDATE_S`.

3. **[README.md](README.md)** (section « Winter mode », lignes ~217-228) : décrire la nouvelle condition de gel, les cycles, les heures creuses, le gel sévère et le repli. Corriger les seuils 4/5 en 3/4.

## Hors périmètre
- **Aucun nouveau paramètre** : tout est en constantes dans `config.h`, comme `FROST_*` aujourd'hui. Conséquence : le JSON `PARAM` ne grossit pas (il est borné à 768 octets, et à 1024 pour le paquet MQTT), et il n'y a rien à toucher dans la discovery HA, `settings.html` / `settings.js` ou `data/`. **Changer les durées ou les heures creuses demande donc un reflash.**
- Côté HA : aucun changement de dashboard. L'entité « Frost Protection » (`frost_active`) signifie « protection engagée » ; l'entité « Filter Pump » montre la pompe réelle, qui suit le cycle.
- Le numéro de version reste inchangé. Un commit « Bump version to 2.3 » séparé reste possible, comme pour 2.2.

## Points à valider
- **Constantes ou paramètres HA** : le plan choisit les constantes. Les passer en paramètres ajouterait ~6 clés au JSON `PARAM` (risque de dépasser 768 octets), plus les entités HA et les champs de la page web.
- **Repli de sécurité sans aucune donnée** (ni extérieur ni sonde valide) : le plan déclare le gel et fait tourner la pompe en cycles. Aujourd'hui, rien ne se passe dans ce cas.
- **Gel sévère avec condition eau** : comme vous l'avez choisi, le gel sévère exige quand même une eau ≤ 3 °C. Si la sonde est dans le bassin et que l'eau reste à 5 °C par -10 °C, la pompe ne démarre pas, et la canalisation peut geler avant que l'eau du bassin ne baisse. La lecture `raw` (canalisation stagnante) atténue ce risque si la sonde est bien dans la canalisation, mais je ne peux pas le vérifier depuis le code.

## Vérification
1. `pio run` : le firmware doit compiler sans avertissement nouveau.
2. Banc d'essai sans attendre le froid : `CMD/SET/WINTER_MODE` à `ON`, `CMD/OUTDOOR_TEMP` à `-2`, puis `CMD/MEASURES` avec `water_temperature_raw` à `2`. Le mode virtuel de `CMD/MEASURES` reste actif jusqu'au reboot. Attendre au plus 10 s, puis surveiller `STATE_FILTER` : `filter_control_state` = 5 et `filter_pump` = 1.
3. Cycle : la pompe doit s'arrêter au bout de 15 min (30 min entre 23 h et 7 h), rester arrêtée le reste de l'heure, puis repartir. Pour aller plus vite, compiler un build d'essai avec `FROST_CYCLE_ON_MIN` à 1 (à rendre surchargeable par `#ifndef` pour ce test).
4. Eau chaude : republier `water_temperature_raw` à `8` : le gel doit être levé en moins de 10 s (log « Hors-gel OFF »), pompe éteinte hors programme.
5. Sévère : `OUTDOOR_TEMP` à `-8` avec raw à `2` : la pompe doit rester allumée sans cycle.
6. Sonde en erreur : `water_temperature_raw` à `255` avec extérieur à `5` : pas de gel. Sans `OUTDOOR_TEMP` récent, le gel est déclaré par sécurité.
7. OFF manuel : `frost_alert` levé, pompe éteinte.
