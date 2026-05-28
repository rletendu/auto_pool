// AutoPool — shared helpers + live dashboard

const $ = (sel, root = document) => root.querySelector(sel);
const $$ = (sel, root = document) => Array.from(root.querySelectorAll(sel));

const api = {
  get: async (path) => {
    const r = await fetch(path, { cache: "no-store" });
    if (!r.ok) throw new Error(`${path} → ${r.status}`);
    return r.json();
  },
  post: async (path, body) => {
    const r = await fetch(path, {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: typeof body === "string" ? body : JSON.stringify(body),
    });
    if (!r.ok) throw new Error(`${path} → ${r.status}`);
    return r;
  },
  cmd: async (path) => {
    const r = await fetch(path, { cache: "no-store" });
    if (!r.ok) throw new Error(`${path} → ${r.status}`);
    return r;
  },
};

function ensureToastStack() {
  let stack = $(".toast-stack");
  if (!stack) {
    stack = document.createElement("div");
    stack.className = "toast-stack";
    document.body.appendChild(stack);
  }
  return stack;
}

function toast(msg, kind = "ok", ttl = 2500) {
  const stack = ensureToastStack();
  const el = document.createElement("div");
  el.className = `toast toast--${kind}`;
  el.textContent = msg;
  stack.appendChild(el);
  setTimeout(() => {
    el.classList.add("toast--leaving");
    setTimeout(() => el.remove(), 200);
  }, ttl);
}

const FILTER_MODE = { 0: "OFF", 1: "ON", 2: "AUTO" };
const FILTER_PWR  = { 0: "FULL", 1: "REG" };

function setMode(toggleEl, value) {
  $$("button", toggleEl).forEach(b => {
    b.setAttribute("aria-pressed", b.dataset.mode === value ? "true" : "false");
  });
}

function setPump(pillEl, on) {
  pillEl.dataset.state = on ? "on" : "off";
  pillEl.textContent = on ? "Running" : "Idle";
}

function setLevel(levelEl, ok) {
  const pill = $(".pump-pill", levelEl);
  pill.dataset.state = ok ? "on" : "warn";
  pill.textContent = ok ? "OK" : "Low";
}

function fmt(value, digits = 1, fallback = "—") {
  if (value === undefined || value === null || Number.isNaN(value)) return fallback;
  return Number(value).toFixed(digits);
}

// ---------------- Live dashboard ----------------

let lastOk = 0;

async function refreshAll() {
  const results = await Promise.allSettled([
    api.get("getmeasures"),
    api.get("getfilterstate"),
    api.get("getorpstate"),
    api.get("getphstate"),
  ]);

  const [meas, filter, orp, ph] = results;
  let anyOk = false;

  if (meas.status === "fulfilled") {
    anyOk = true;
    renderMeasures(meas.value);
  }
  if (filter.status === "fulfilled") {
    anyOk = true;
    renderFilter(filter.value);
  }
  if (orp.status === "fulfilled") {
    anyOk = true;
    renderOrp(orp.value);
  }
  if (ph.status === "fulfilled") {
    anyOk = true;
    renderPh(ph.value);
  }

  const dot = $(".status-dot");
  if (anyOk) {
    lastOk = Date.now();
    if (dot) dot.dataset.status = "ok";
  } else if (dot) {
    dot.dataset.status = "err";
  }
}

function renderMeasures(m) {
  $("#m-water").textContent       = fmt(m.water_temperature, 1);
  $("#m-ph").textContent          = fmt(m.ph, 2);
  $("#m-orp").textContent         = fmt(m.orp, 0);
  $("#m-pressure").textContent    = fmt(m.pump_pressure, 2);
  $("#m-air-temp").textContent    = fmt(m.system_temperature, 1);
  $("#m-air-hum").textContent     = fmt(m.system_humidity, 0);
  $("#m-day-max").textContent     = fmt(m.day_max_water_temperature, 1);
  $("#m-daily-cl").textContent    = fmt(m.daily_ml_orp, 0);
  $("#m-daily-phm").textContent   = fmt(m.daily_ml_ph_minus, 0);
  $("#m-daily-filter").textContent  = fmt(m.daily_filter_min, 0);
  $("#m-total-filter").textContent  = m.total_filter_min == null ? "—" : (m.total_filter_min / 60).toFixed(1);

  const set = (id, ok) => {
    const el = $(id);
    if (el) setLevel(el, !!ok);
  };
  set("#lvl-water",   m.level_water);
  set("#lvl-cl",      m.level_cl);
  set("#lvl-ph-m",    m.level_ph_minus);
  set("#lvl-ph-p",    m.level_ph_plus);
}

function renderFilter(s) {
  const mode = FILTER_MODE[s.filter_mode] || "—";
  const pwr  = FILTER_PWR[s.filter_power] || "—";
  setMode($("#filter-mode-toggle"), mode);
  setMode($("#filter-power-toggle"), pwr);
  setPump($("#filter-pump"), !!s.filter_pump);
}

function renderOrp(s) {
  const mode = FILTER_MODE[s.orp_mode] || "—";
  setMode($("#orp-mode-toggle"), mode);
  setPump($("#orp-pump"), !!s.orp_pump);
}

function renderPh(s) {
  const mode = FILTER_MODE[s.ph_minus_mode] || "—";
  setMode($("#ph-mode-toggle"), mode);
  setPump($("#ph-pump"), !!s.ph_minus_pump);
}

const CMD_MAP = {
  filter: { OFF: "filter_off", ON: "filter_on", AUTO: "filter_auto" },
  fpower: { FULL: "filter_pwr_full", REG: "filter_pwr_reg" },
  orp:    { OFF: "orp_off", ON: "orp_on", AUTO: "orp_auto" },
  ph:     { OFF: "ph_off", ON: "ph_on", AUTO: "ph_auto" },
};

function bindModeToggle(toggleEl, group, label) {
  if (!toggleEl) return;
  $$("button", toggleEl).forEach(btn => {
    btn.addEventListener("click", async () => {
      const mode = btn.dataset.mode;
      const path = CMD_MAP[group]?.[mode];
      if (!path) return;
      // optimistic
      setMode(toggleEl, mode);
      try {
        await api.cmd(path);
        toast(`${label} → ${mode}`, "ok");
        // confirm with a refresh on next tick
        setTimeout(refreshAll, 200);
      } catch (e) {
        toast(`${label}: network error`, "err", 4000);
      }
    });
  });
}

document.addEventListener("DOMContentLoaded", () => {
  bindModeToggle($("#filter-mode-toggle"),  "filter", "Filter");
  bindModeToggle($("#filter-power-toggle"), "fpower", "Filter power");
  bindModeToggle($("#orp-mode-toggle"),     "orp",    "ORP");
  bindModeToggle($("#ph-mode-toggle"),      "ph",     "pH");

  refreshAll();
  setInterval(refreshAll, 4000);

  // Stale-check: if no successful poll in 12s, mark dot warn
  setInterval(() => {
    const dot = $(".status-dot");
    if (!dot) return;
    const age = Date.now() - lastOk;
    if (lastOk && age > 12000 && dot.dataset.status === "ok") {
      dot.dataset.status = "warn";
    }
  }, 2000);
});
