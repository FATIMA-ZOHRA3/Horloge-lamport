const $ = (id) => document.getElementById(id);
const S = { data: null, clock: "lamport", sel: null, source: "reference", since: 0, poll: null };
const COLORS = { LOCAL: "#64748b", SEND: "#fbbf24", RECV: "#2dd4bf" };
const esc = (s) => String(s ?? "").replace(/[&<>"']/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;" }[c]));
const fmtV = (v) => (v ? "[" + v.join(", ") + "]" : "—");

function leq(a, b) {
  const n = Math.max(a.length, b.length);
  for (let i = 0; i < n; i++) if ((a[i] || 0) > (b[i] || 0)) return false;
  return true;
}
function rel(a, b) {
  if (!a || !b) return "unknown";
  if (a.join() === b.join()) return "equal";
  if (leq(a, b)) return "before";
  if (leq(b, a)) return "after";
  return "concurrent";
}

/* ---------- Navigation ---------- */
document.querySelectorAll(".nav").forEach((b) =>
  b.addEventListener("click", () => {
    document.querySelectorAll(".nav").forEach((x) => x.classList.toggle("active", x === b));
    document.querySelectorAll(".tab").forEach((t) => t.classList.toggle("active", t.id === "tab-" + b.dataset.tab));
  })
);
function goTab(name) { document.querySelector(`.nav[data-tab="${name}"]`).click(); }

document.querySelectorAll('input[name="clock"]').forEach((r) =>
  r.addEventListener("change", () => { S.clock = r.value; renderTimeline(); })
);

/* ---------- Chargement ---------- */
async function loadSources() {
  const list = await (await fetch("/api/sources")).json();
  $("source").innerHTML = list.map((s) => `<option value="${s.id}">${esc(s.label)} (${s.files} CSV)</option>`).join("");
  $("source").value = S.source;
}
async function loadData() {
  S.data = await (await fetch("/api/data?source=" + S.source)).json();
  S.sel = null;
  renderAll();
}
async function checkDocker() {
  const r = await (await fetch("/api/docker")).json();
  const c = $("dockerChip");
  c.textContent = r.ok ? "Docker : actif" : "Docker : inactif";
  c.className = "chip " + (r.ok ? "ok" : "ko");
}
$("source").addEventListener("change", (e) => { S.source = e.target.value; loadData(); });
$("btnRefresh").addEventListener("click", async () => { await loadSources(); await loadData(); checkDocker(); });

/* ---------- Rendu global ---------- */
function renderAll() {
  const d = S.data;
  $("subtitle").textContent = d.events.length
    ? `${d.processes.length} processus · ${d.events.length} événements · ${d.messages.length} messages`
    : "Aucun journal CSV trouvé dans cette source.";
  const st = d.stats;
  const kpi = (l, v) => `<div class="kpi"><small>${l}</small><b>${v}</b></div>`;
  $("kpis").innerHTML =
    kpi("Processus", d.processes.length) + kpi("Événements", st.events) + kpi("Messages échangés", st.messages) +
    kpi("Paires concurrentes", d.has_vector ? st.concurrent_pairs : "—") +
    kpi("Faux ordres (Lamport)", d.has_vector ? st.lamport_false_order : "—");
  $("warnings").innerHTML = d.warnings.map((w) => `<div class="warn">${esc(w)}</div>`).join("");
  renderTimeline(); renderCompare(); renderCausal(); renderEvents();
}

/* ---------- Chronogramme ---------- */
function renderTimeline() {
  const d = S.data, box = $("timeline");
  if (!d || !d.events.length) { box.innerHTML = '<div class="empty">Aucun événement à afficher.</div>'; return; }
  const X0 = 110, DX = 84, Y0 = 70, DY = 110;
  const maxc = Math.max(...d.events.map((e) => e.col));
  const W = X0 + (maxc + 1) * DX + 40, H = Y0 + (d.processes.length - 1) * DY + 70;
  const row = Object.fromEntries(d.processes.map((p, i) => [p, i]));
  const pos = (e) => [X0 + e.col * DX, Y0 + row[e.proc] * DY];
  const byId = Object.fromEntries(d.events.map((e) => [e.id, e]));
  const sel = S.sel !== null ? byId[S.sel] : null;

  let s = `<svg width="${W}" height="${H}" viewBox="0 0 ${W} ${H}">
    <defs><marker id="arr" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse">
    <path d="M0 0L10 5L0 10z" fill="#8a9ab8"/></marker></defs>`;
  d.processes.forEach((p, i) => {
    const y = Y0 + i * DY;
    s += `<line x1="${X0 - 30}" y1="${y}" x2="${W - 20}" y2="${y}" stroke="#24334f" stroke-width="2"/>
          <text x="20" y="${y + 5}" fill="#e6ecf8" font-size="15" font-weight="600">P${esc(p)}</text>`;
  });
  d.messages.forEach(([a, b]) => {
    const [x1, y1] = pos(byId[a]), [x2, y2] = pos(byId[b]);
    s += `<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="#8a9ab8" stroke-width="1.6" marker-end="url(#arr)" opacity=".85"/>`;
  });
  d.events.forEach((e) => {
    const [x, y] = pos(e);
    let color = COLORS[e.type], r = 9;
    if (sel && d.has_vector) {
      const rl = e.id === sel.id ? "self" : rel(e.vector, sel.vector);
      if (rl === "self") { color = "#ffffff"; r = 11; }
      else if (rl === "before") color = "#4f8cff";
      else if (rl === "after") color = "#4ade80";
      else if (rl === "concurrent") color = "#fb923c";
    }
    const label = S.clock === "lamport" ? (e.lamport ?? "—") : fmtV(e.vector);
    const up = row[e.proc] % 2 === 0;
    s += `<circle class="ev" data-id="${e.id}" cx="${x}" cy="${y}" r="${r}" fill="${color}"/>
          <text x="${x}" y="${y + (up ? -18 : 28)}" fill="#e6ecf8" font-size="11.5" text-anchor="middle">${esc(label)}</text>`;
  });
  box.innerHTML = s + "</svg>";
  box.querySelectorAll(".ev").forEach((c) =>
    c.addEventListener("click", () => { S.sel = Number(c.dataset.id); renderTimeline(); showInfo(); })
  );
  showInfo();
}
function showInfo() {
  const d = S.data, el = $("eventInfo");
  if (S.sel === null) { el.textContent = ""; return; }
  const e = d.events.find((x) => x.id === S.sel);
  let p = 0, f = 0, c = 0;
  if (d.has_vector) d.events.forEach((o) => {
    if (o.id === e.id) return;
    const r = rel(o.vector, e.vector);
    if (r === "before") p++; else if (r === "after") f++; else if (r === "concurrent") c++;
  });
  el.innerHTML = `<b>P${esc(e.proc)} · ${esc(e.type)}</b> — t = ${esc(e.time ?? "—")} ms · Lamport = ${esc(e.lamport ?? "—")} · Vecteur = ${esc(fmtV(e.vector))}`
    + (e.msg ? ` · msg ${esc(e.msg)}` : "")
    + (d.has_vector ? ` · passé : <b>${p}</b> · futur : <b>${f}</b> · concurrents : <b>${c}</b>` : "");
}

/* ---------- Comparaison ---------- */
function renderCompare() {
  const d = S.data, st = d.stats;
  const card = (t, v, txt) => `<div class="card"><small>${t}</small><b>${v}</b><p>${txt}</p></div>`;
  $("compareCards").innerHTML = d.has_vector
    ? card("Paires concurrentes", st.concurrent_pairs, "Événements sans lien de causalité d'après les horloges vectorielles.")
    + card("Faux ordres imposés par Lamport", st.lamport_false_order, "Paires concurrentes que Lamport classe pourtant avec des dates différentes (il ne peut pas les distinguer d'un vrai ordre causal).")
    + card("Violations de cohérence", st.lamport_violations, "Doit rester à 0 : si e → f alors L(e) < L(f).")
    : '<div class="empty">Pas d\'horloges vectorielles dans ces journaux.</div>';

  // Histogramme empilé par processus
  const pp = d.per_proc, max = Math.max(1, ...pp.map((p) => p.total));
  const BW = 70, H = 220, W = 60 + pp.length * (BW + 40);
  let s = `<svg width="${W}" height="${H + 50}">`;
  pp.forEach((p, i) => {
    const x = 50 + i * (BW + 40);
    let y = H;
    [["local", "#64748b"], ["send", "#fbbf24"], ["recv", "#2dd4bf"]].forEach(([k, col]) => {
      const h = (p[k] / max) * (H - 20);
      y -= h;
      s += `<rect x="${x}" y="${y}" width="${BW}" height="${h}" fill="${col}" rx="3"/>`;
      if (p[k]) s += `<text x="${x + BW / 2}" y="${y + h / 2 + 4}" text-anchor="middle" font-size="11" fill="#0b1220">${p[k]}</text>`;
    });
    s += `<text x="${x + BW / 2}" y="${H + 20}" text-anchor="middle" fill="#e6ecf8" font-size="13">P${esc(p.proc)}</text>`;
  });
  $("barChart").innerHTML = s + "</svg>";

  $("procTable").innerHTML =
    "<tr><th>Processus</th><th>Événements</th><th>Locaux</th><th>Envois</th><th>Réceptions</th><th>Lamport max</th><th>Dernier vecteur</th></tr>" +
    pp.map((p) => `<tr><td>P${esc(p.proc)}</td><td>${p.total}</td><td>${p.local}</td><td>${p.send}</td><td>${p.recv}</td><td>${esc(p.max_lamport ?? "—")}</td><td>${esc(fmtV(p.last_vector))}</td></tr>`).join("");
}

/* ---------- Causalité ---------- */
function renderCausal() {
  const d = S.data, byId = Object.fromEntries(d.events.map((e) => [e.id, e]));
  const nm = (e) => `P${e.proc} · ${e.type} · L=${e.lamport ?? "—"} · V=${fmtV(e.vector)}`;
  $("concTable").innerHTML = "<tr><th>#</th><th>Événement A</th><th>Événement B</th></tr>" +
    (d.stats.conc_pairs.length
      ? d.stats.conc_pairs.map(([a, b], i) => `<tr><td>${i + 1}</td><td>${esc(nm(byId[a]))}</td><td>${esc(nm(byId[b]))}</td></tr>`).join("")
      : '<tr><td colspan="3">Aucune paire concurrente détectée.</td></tr>');
}
$("btnCausal").addEventListener("click", async () => {
  $("causalOut").textContent = "Exécution…";
  const r = await fetch("/api/causalite", { method: "POST" });
  $("causalOut").textContent = (await r.json()).output || "(aucune sortie)";
});

/* ---------- Événements ---------- */
function renderEvents() {
  $("eventTable").innerHTML = "<tr><th>#</th><th>Fichier</th><th>Processus</th><th>Type</th><th>Temps (ms)</th><th>Lamport</th><th>Vecteur</th><th>msg_id</th><th>Pair</th></tr>" +
    S.data.events.map((e) => `<tr><td>${e.id}</td><td>${esc(e.file)}</td><td>P${esc(e.proc)}</td><td>${esc(e.type)}</td><td>${esc(e.time ?? "—")}</td><td>${esc(e.lamport ?? "—")}</td><td>${esc(fmtV(e.vector))}</td><td>${esc(e.msg ?? "—")}</td><td>${esc(e.peer ?? "—")}</td></tr>`).join("");
}

/* ---------- Lancement Docker ---------- */
function setRunning(running) {
  ["btnLamport", "btnVector"].forEach((id) => ($(id).disabled = running));
  $("btnStop").disabled = !running;
}
async function launch(mode) {
  const body = { mode, scenario: $("scenario").value, n: Number($("nproc").value) };
  const r = await fetch("/api/run", { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(body) });
  if (!r.ok) { alert((await r.json()).error); return; }
  S.since = 0; $("console").textContent = "";
  setRunning(true); goTab("console");
  clearInterval(S.poll); S.poll = setInterval(pollStatus, 800);
}
async function pollStatus() {
  const st = await (await fetch("/api/status?since=" + S.since)).json();
  if (st.lines.length) {
    const c = $("console");
    c.textContent += st.lines.join("\n") + "\n";
    c.scrollTop = c.scrollHeight;
    S.since = st.total;
  }
  const chip = $("runStatus");
  chip.textContent = st.status;
  chip.className = "chip " + (st.status === "running" ? "run" : st.status === "done" ? "ok" : st.status === "error" ? "ko" : "");
  if (!["running", "stopping"].includes(st.status)) {
    clearInterval(S.poll); setRunning(false);
    if (st.status === "done") {
      await loadSources(); S.source = "docker"; $("source").value = "docker";
      await loadData(); goTab("timeline");
    }
  }
}
$("btnLamport").addEventListener("click", () => launch("lamport"));
$("btnVector").addEventListener("click", () => launch("vector"));
$("btnStop").addEventListener("click", () => fetch("/api/stop", { method: "POST" }));
/* ---------- Scénarios ---------- */
async function loadScenarios() {
  const r = await (await fetch("/api/scenarios")).json();
  $("scenario").innerHTML = r.scenarios.map((n) => `<option value="${esc(n)}">${esc(n)}</option>`).join("");
  if (r.scenarios.includes(r.default)) $("scenario").value = r.default;
  syncN();
}
// Déduit le nombre de processus du nom (ex. chaine_3 -> 3) ; tu peux le corriger à la main
function syncN() {
  const m = $("scenario").value.match(/(\d+)$/);
  if (m) $("nproc").value = m[1];
}
async function fillScenario() {
  const r = await fetch("/api/scenario?name=" + encodeURIComponent($("scenario").value));
  $("scenPreview").textContent = (await r.json()).content;
}
$("scenario").addEventListener("change", () => { syncN(); if (!$("scenPreview").hidden) fillScenario(); });
$("btnScenView").addEventListener("click", async () => {
  const box = $("scenPreview");
  if (!box.hidden) { box.hidden = true; return; }
  await fillScenario();
  box.hidden = false;
});
/* ---------- Démarrage ---------- */
(async () => {
  setRunning(false); checkDocker();
  await loadScenarios();
  await loadSources(); await loadData();
})();