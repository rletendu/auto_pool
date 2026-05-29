// AutoPool — settings page logic
// Depends on app.js for $, $$, api, toast.

const DEFAULT_TEMP_TABLE = [
  264192, 264192, 329728, 329728, 460800, 460800,
  493568, 494592, 510976, 1043456, 2092544,
];

let params = null;

// --------------- Build dynamic timer grids ---------------

function buildHourGrid(container, idPrefix) {
  // 24 checkboxes
  const frag = document.createDocumentFragment();
  for (let h = 0; h < 24; h++) {
    const cb = document.createElement("input");
    cb.type = "checkbox";
    cb.id = `${idPrefix}${h}`;
    cb.title = `${String(h).padStart(2, "0")}:00`;
    frag.appendChild(cb);
  }
  container.appendChild(frag);
}

function buildHourAxis(container) {
  const frag = document.createDocumentFragment();
  for (let h = 0; h < 24; h++) {
    const span = document.createElement("span");
    if (h % 6 === 0) {
      span.dataset.major = "true";
      span.textContent = String(h);
    }
    frag.appendChild(span);
  }
  container.appendChild(frag);
}

function buildTempTimerTable(container) {
  // grid: temp label | 24 checkboxes | hour count
  const frag = document.createDocumentFragment();

  const emptyLabel = document.createElement("div");
  frag.appendChild(emptyLabel);
  const axis = document.createElement("div");
  axis.className = "timer-axis";
  for (let h = 0; h < 24; h++) {
    const s = document.createElement("span");
    if (h % 6 === 0) { s.dataset.major = "true"; s.textContent = h; }
    axis.appendChild(s);
  }
  frag.appendChild(axis);

  for (let t = 10; t <= 30; t += 2) {
    const label = document.createElement("div");
    label.className = "temp-label";
    label.dataset.temp = t;
    label.textContent = `${t}°C`;
    const grid = document.createElement("div");
    grid.className = "timer-grid";
    grid.dataset.temp = t;
    for (let h = 0; h < 24; h++) {
      const cb = document.createElement("input");
      cb.type = "checkbox";
      cb.id = `t${t}_h${h}`;
      cb.title = `${t}°C @ ${String(h).padStart(2, "0")}:00`;
      grid.appendChild(cb);
    }
    const count = document.createElement("span");
    count.className = "temp-hour-count";
    count.id = `tc${t}`;
    frag.appendChild(label);
    frag.appendChild(grid);
    frag.appendChild(count);
  }
  container.appendChild(frag);
  container.addEventListener("change", updateTempCounts);
}

// --------------- Param ↔ DOM ---------------

const SCALAR_FIELDS = [
  "target_ph", "delta_ph",
  "target_orp", "delta_orp",
  "flow_cl", "flow_ph_minus", "flow_ph_plus",
  "pressure_warning",
  "cl_max_day", "phm_max_day",
  "orp_offset", "ph_offset",
  "periodic_filter_time",
  "ha_discovery_prefix",
  "device_suffix",
];

function paramsToForm(p) {
  for (const k of SCALAR_FIELDS) {
    const el = $(`#${k}`);
    if (el && p[k] !== undefined) el.value = p[k];
  }
  $("#ha_discovery_enabled").checked = !!p.ha_discovery_enabled;

  const tprog = p.timer_prog | 0;
  for (let h = 0; h < 24; h++) {
    const el = $(`#h${h}`);
    if (el) el.checked = (tprog >> h) & 1;
  }
  const tt = p.timer_prog_temperature || [];
  for (let i = 0; i < 11; i++) {
    const t = 10 + i * 2;
    const val = tt[i] | 0;
    for (let h = 0; h < 24; h++) {
      const el = $(`#t${t}_h${h}`);
      if (el) el.checked = (val >> h) & 1;
    }
  }

  if (p.filter_auto_mode === 1) {
    $("#FilterTimerTemperature").checked = true;
  } else {
    $("#FilterTimerFixed").checked = true;
  }
}

function formToParams() {
  const out = Object.assign({}, params);
  for (const k of SCALAR_FIELDS) {
    const el = $(`#${k}`);
    if (!el) continue;
    if (el.type === "number") {
      out[k] = parseFloat(el.value);
    } else {
      out[k] = el.value;
    }
  }
  out.ha_discovery_enabled = $("#ha_discovery_enabled").checked;

  let tprog = 0;
  for (let h = 0; h < 24; h++) {
    const el = $(`#h${h}`);
    if (el && el.checked) tprog |= (1 << h);
  }
  out.timer_prog = tprog;

  const tt = [];
  for (let i = 0; i < 11; i++) {
    const t = 10 + i * 2;
    let val = 0;
    for (let h = 0; h < 24; h++) {
      const el = $(`#t${t}_h${h}`);
      if (el && el.checked) val |= (1 << h);
    }
    tt.push(val);
  }
  out.timer_prog_temperature = tt;
  out.filter_auto_mode = $("#FilterTimerTemperature").checked ? 1 : 0;
  return out;
}

// --------------- Temperature hour counts ---------------

function updateTempCounts() {
  for (let t = 10; t <= 30; t += 2) {
    let n = 0;
    for (let h = 0; h < 24; h++) {
      if ($(`#t${t}_h${h}`)?.checked) n++;
    }
    const el = $(`#tc${t}`);
    if (el) el.textContent = n ? `${n}h` : "—";
  }
}

// --------------- Live state highlighting ---------------

async function highlightCurrentState() {
  // Current hour — outline the matching checkbox in every grid
  const hour = new Date().getHours();
  $$(".timer-grid input.current-hour").forEach((el) =>
    el.classList.remove("current-hour")
  );
  $(`#h${hour}`)?.classList.add("current-hour");
  for (let t = 10; t <= 30; t += 2)
    $(`#t${t}_h${hour}`)?.classList.add("current-hour");

  // Current temp row — only relevant in temperature mode
  $$("[data-temp]").forEach((el) => el.classList.remove("current-temp-row"));
  if (params?.filter_auto_mode === 1) {
    try {
      const m = await api.get("getmeasures");
      const raw = m.day_max_water_temperature ?? 0;
      const idx = Math.min(10, Math.max(0, Math.floor((raw - 10) / 2)));
      const activeTemp = 10 + idx * 2;
      $$(`[data-temp="${activeTemp}"]`).forEach((el) =>
        el.classList.add("current-temp-row")
      );
    } catch (_) {}
  }
}

// --------------- Actions ---------------

async function loadParams() {
  try {
    params = await api.get("getparameters");
    paramsToForm(params);
    updateTempCounts();
    highlightCurrentState();
  } catch (e) {
    toast("Failed to load parameters", "err", 4000);
  }
}

async function saveParams() {
  const payload = formToParams();
  try {
    await api.post("setparameters", payload);
    params = payload;
    toast("Settings saved", "ok");
  } catch (e) {
    toast(`Save failed: ${e.message}`, "err", 4000);
  }
}

async function rebootDevice() {
  if (!confirm("Reboot the controller now?")) return;
  try {
    await api.cmd("reboot");
    toast("Rebooting…", "warn", 4000);
  } catch (e) {
    toast(`Reboot failed: ${e.message}`, "err", 4000);
  }
}

async function resetDailyCl() {
  if (!confirm("Reset the daily chlorine counter?")) return;
  try {
    await api.cmd("rst_daily_ml_orp");
    toast("Daily Cl counter reset", "ok");
  } catch (e) {
    toast(`Reset failed: ${e.message}`, "err", 4000);
  }
}

async function resetBootCount() {
  if (!confirm("Reset the boot counter?")) return;
  try {
    await api.cmd("rst_boot_count");
    toast("Boot counter reset", "ok");
  } catch (e) {
    toast(`Reset failed: ${e.message}`, "err", 4000);
  }
}

function copyDailyToTemp() {
  if (!confirm("Copy fixed schedule into every temperature row?")) return;
  let fixed = 0;
  for (let h = 0; h < 24; h++) {
    if ($(`#h${h}`).checked) fixed |= (1 << h);
  }
  for (let t = 10; t <= 30; t += 2) {
    for (let h = 0; h < 24; h++) {
      $(`#t${t}_h${h}`).checked = !!((fixed >> h) & 1);
    }
  }
  updateTempCounts();
}

function loadDefaultTempTable() {
  if (!confirm("Replace the temperature table with the default?")) return;
  for (let i = 0; i < 11; i++) {
    const t = 10 + i * 2;
    const val = DEFAULT_TEMP_TABLE[i];
    for (let h = 0; h < 24; h++) {
      $(`#t${t}_h${h}`).checked = !!((val >> h) & 1);
    }
  }
  updateTempCounts();
}

// --------------- TFT upload ---------------

function bindTftUpload() {
  const form = $("#tft-form");
  const fileInput = $("#tft-file");
  const submitBtn = $("#tft-submit");
  const progress = $("#tft-progress");
  const progressBar = $("#tft-progress-bar");
  const progressText = $("#tft-progress-text");
  if (!form) return;

  fileInput.addEventListener("change", async () => {
    const f = fileInput.files[0];
    if (!f) return;
    submitBtn.disabled = true;
    try {
      // Pre-flight ESP fileSize endpoint
      await fetch("/fs", {
        method: "POST",
        headers: { "Content-Type": "application/x-www-form-urlencoded" },
        body: `fileSize=${f.size}`,
      });
      submitBtn.disabled = false;
      progressText.textContent = `${(f.size / 1024).toFixed(1)} KB ready`;
    } catch (e) {
      toast(`Pre-flight failed: ${e.message}`, "err", 4000);
    }
  });

  form.addEventListener("submit", (e) => {
    e.preventDefault();
    const f = fileInput.files[0];
    if (!f) {
      toast("Pick a .tft file first", "warn");
      return;
    }
    progress.hidden = false;
    progressBar.style.width = "0%";
    progressText.textContent = "Uploading…";
    submitBtn.disabled = true;

    const xhr = new XMLHttpRequest();
    xhr.open("POST", "settings.html");
    xhr.upload.addEventListener("progress", (ev) => {
      if (ev.lengthComputable) {
        const pct = (ev.loaded / ev.total) * 100;
        progressBar.style.width = `${pct}%`;
        progressText.textContent = `${pct.toFixed(0)}% — ${(ev.loaded / 1024).toFixed(0)} / ${(ev.total / 1024).toFixed(0)} KB`;
      }
    });
    xhr.addEventListener("load", () => {
      progressText.textContent = "Done — controller restarting";
      toast("Display firmware uploaded", "ok", 4000);
    });
    xhr.addEventListener("error", () => {
      progressText.textContent = "Upload failed";
      toast("Upload failed", "err", 4000);
      submitBtn.disabled = false;
    });
    const fd = new FormData();
    fd.append("name", f);
    xhr.send(fd);
  });
}

// --------------- Boot ---------------

document.addEventListener("DOMContentLoaded", () => {
  buildHourAxis($("#hour-axis"));
  buildHourGrid($("#hour-grid"), "h");
  buildTempTimerTable($("#temp-timer-table"));

  $("#btn-save").addEventListener("click", saveParams);
  $("#btn-reboot").addEventListener("click", rebootDevice);
  $("#btn-reset-cl").addEventListener("click", resetDailyCl);
  $("#btn-reset-boot").addEventListener("click", resetBootCount);
  $("#btn-copy-fixed").addEventListener("click", copyDailyToTemp);
  $("#btn-load-defaults").addEventListener("click", loadDefaultTempTable);

  bindTftUpload();
  loadParams();
  setInterval(highlightCurrentState, 60_000);
});
