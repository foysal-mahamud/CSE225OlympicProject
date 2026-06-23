(() => {
  'use strict';

  const PAGE_SIZE = 25;

  const state = {
    rows: [],
    filtered: [],
    page: 0,
    search: '',
    medal: '',
  };

  // ---------- CSV parsing ----------

  // RFC4180-style parser that handles quoted fields with embedded commas.
  function parseCSV(text) {
    const rows = [];
    let row = [];
    let field = '';
    let inQuotes = false;
    for (let i = 0; i < text.length; i++) {
      const c = text[i];
      if (inQuotes) {
        if (c === '"') {
          if (text[i + 1] === '"') { field += '"'; i++; }
          else { inQuotes = false; }
        } else {
          field += c;
        }
      } else {
        if (c === '"') {
          inQuotes = true;
        } else if (c === ',') {
          row.push(field);
          field = '';
        } else if (c === '\n') {
          row.push(field);
          rows.push(row);
          row = [];
          field = '';
        } else if (c === '\r') {
          // skip
        } else {
          field += c;
        }
      }
    }
    if (field.length || row.length) {
      row.push(field);
      rows.push(row);
    }
    return rows;
  }

  // Clean Python-list-looking strings such as ['Wrestling'] or
  // ["Men's Greco-Roman 97kg"] into a comma-separated readable string.
  function cleanList(value) {
    if (!value) return '';
    let v = value.trim();
    if ((v.startsWith('[') && v.endsWith(']'))) {
      v = v.slice(1, -1);
    }
    return v
      .split(/,(?![^']*')/)
      .map(s => s.trim().replace(/^['"]|['"]$/g, ''))
      .filter(Boolean)
      .join(', ');
  }

  function buildRows(raw) {
    if (raw.length === 0) return [];
    const headers = raw[0].map(h => h.trim());
    const idx = (name) => headers.findIndex(h => h.toLowerCase() === name.toLowerCase());
    const ATHLETE_ID = idx('Athlete ID');
    const NAME = idx('Name');
    const GENDER = idx('Gender');
    const COUNTRY_NAME = idx('Country Name');
    const COUNTRY_CODE = idx('Country Code');
    const EVENT_TYPE = idx('Event Type');
    const EVENT_NAME = idx('Event Name');
    const MEDAL_TYPE = idx('Medal Type');
    const VENUE = idx('Venue');
    const GAME = idx('Game');

    const rows = [];
    for (let i = 1; i < raw.length; i++) {
      const r = raw[i];
      if (!r || r.every(c => !c || !c.trim())) continue;
      rows.push({
        athleteID: r[ATHLETE_ID] || '',
        name: r[NAME] || '',
        gender: r[GENDER] || '',
        country: r[COUNTRY_NAME] || '',
        countryCode: r[COUNTRY_CODE] || '',
        sport: cleanList(r[EVENT_TYPE]) || '',
        event: cleanList(r[EVENT_NAME]) || '',
        medal: (r[MEDAL_TYPE] || '').trim(),
        venue: r[VENUE] || '',
        game: r[GAME] || '',
      });
    }
    return rows;
  }

  // ---------- KPIs ----------

  function renderKPIs(rows) {
    const athletes = new Set();
    const countries = new Set();
    const sports = new Set();
    let medals = 0;
    for (const r of rows) {
      if (r.name) athletes.add(r.name);
      if (r.country) countries.add(r.country);
      if (r.sport) sports.add(r.sport);
      if (r.medal) medals++;
    }
    document.getElementById('kpi-athletes').textContent = athletes.size.toLocaleString();
    document.getElementById('kpi-countries').textContent = countries.size.toLocaleString();
    document.getElementById('kpi-sports').textContent = sports.size.toLocaleString();
    document.getElementById('kpi-medals').textContent = medals.toLocaleString();
  }

  // ---------- Athletes table ----------

  function applyFilters() {
    const q = state.search.trim().toLowerCase();
    const medal = state.medal;
    state.filtered = state.rows.filter(r => {
      if (medal && r.medal !== medal) return false;
      if (!q) return true;
      return (
        r.name.toLowerCase().includes(q) ||
        r.country.toLowerCase().includes(q) ||
        r.sport.toLowerCase().includes(q) ||
        r.event.toLowerCase().includes(q) ||
        r.athleteID.toLowerCase().includes(q)
      );
    });
    state.page = 0;
    renderTable();
  }

  function medalBadge(medal) {
    if (!medal) return '';
    const short = medal.replace(' Medal', '');
    return `<span class="medal-badge medal-${short}">${short}</span>`;
  }

  function renderTable() {
    const tbody = document.querySelector('#athletes-table tbody');
    const start = state.page * PAGE_SIZE;
    const slice = state.filtered.slice(start, start + PAGE_SIZE);
    tbody.innerHTML = slice.map(r => `
      <tr>
        <td>${escapeHTML(r.name)}</td>
        <td>${escapeHTML(r.country)}</td>
        <td>${escapeHTML(r.sport)}</td>
        <td>${escapeHTML(r.event)}</td>
        <td>${medalBadge(r.medal)}</td>
        <td>${escapeHTML(r.venue)}</td>
      </tr>
    `).join('');

    const totalPages = Math.max(1, Math.ceil(state.filtered.length / PAGE_SIZE));
    document.getElementById('page-info').textContent =
      `Page ${state.page + 1} of ${totalPages}`;
    document.getElementById('prev-page').disabled = state.page === 0;
    document.getElementById('next-page').disabled = state.page >= totalPages - 1;
    document.getElementById('result-count').textContent =
      `${state.filtered.length.toLocaleString()} results`;
  }

  function escapeHTML(s) {
    return String(s ?? '')
      .replace(/&/g, '&amp;')
      .replace(/</g, '&lt;')
      .replace(/>/g, '&gt;')
      .replace(/"/g, '&quot;');
  }

  // ---------- Charts ----------

  function tally(rows, key, opts = {}) {
    const onlyMedaled = opts.onlyMedaled || false;
    const m = new Map();
    for (const r of rows) {
      if (onlyMedaled && !r.medal) continue;
      const k = r[key];
      if (!k) continue;
      m.set(k, (m.get(k) || 0) + 1);
    }
    return [...m.entries()].sort((a, b) => b[1] - a[1]);
  }

  function chartTheme() {
    const grid = 'rgba(255,255,255,0.08)';
    const text = '#95a0c8';
    return {
      maintainAspectRatio: false,
      plugins: { legend: { labels: { color: text } } },
      scales: {
        x: { ticks: { color: text }, grid: { color: grid } },
        y: { ticks: { color: text }, grid: { color: grid } },
      },
    };
  }

  function renderCountriesChart(rows) {
    const top = tally(rows, 'country', { onlyMedaled: true }).slice(0, 15);
    new Chart(document.getElementById('chart-countries'), {
      type: 'bar',
      data: {
        labels: top.map(([k]) => k),
        datasets: [{
          label: 'Medals',
          data: top.map(([, v]) => v),
          backgroundColor: '#4d8bff',
        }],
      },
      options: { ...chartTheme(), indexAxis: 'y' },
    });
  }

  function renderMedalsStackChart(rows) {
    const top = tally(rows, 'country', { onlyMedaled: true }).slice(0, 10).map(([k]) => k);
    const gold = {}, silver = {}, bronze = {};
    for (const c of top) { gold[c] = 0; silver[c] = 0; bronze[c] = 0; }
    for (const r of rows) {
      if (!top.includes(r.country)) continue;
      if (r.medal === 'Gold Medal') gold[r.country]++;
      else if (r.medal === 'Silver Medal') silver[r.country]++;
      else if (r.medal === 'Bronze Medal') bronze[r.country]++;
    }
    new Chart(document.getElementById('chart-medals-stack'), {
      type: 'bar',
      data: {
        labels: top,
        datasets: [
          { label: 'Gold',   data: top.map(c => gold[c]),   backgroundColor: '#ffd24a' },
          { label: 'Silver', data: top.map(c => silver[c]), backgroundColor: '#c7cdd6' },
          { label: 'Bronze', data: top.map(c => bronze[c]), backgroundColor: '#cd7f32' },
        ],
      },
      options: {
        ...chartTheme(),
        scales: {
          x: { stacked: true, ticks: { color: '#95a0c8' }, grid: { color: 'rgba(255,255,255,0.08)' } },
          y: { stacked: true, ticks: { color: '#95a0c8' }, grid: { color: 'rgba(255,255,255,0.08)' } },
        },
      },
    });
  }

  function renderAthletesChart(rows) {
    const top = tally(rows, 'name', { onlyMedaled: true }).slice(0, 20);
    new Chart(document.getElementById('chart-athletes'), {
      type: 'bar',
      data: {
        labels: top.map(([k]) => k),
        datasets: [{
          label: 'Medals',
          data: top.map(([, v]) => v),
          backgroundColor: '#ffb43a',
        }],
      },
      options: { ...chartTheme(), indexAxis: 'y' },
    });
  }

  function renderSportsCharts(rows) {
    const sportTally = tally(rows, 'sport', { onlyMedaled: true }).slice(0, 15);
    new Chart(document.getElementById('chart-sports'), {
      type: 'bar',
      data: {
        labels: sportTally.map(([k]) => k),
        datasets: [{ label: 'Medals', data: sportTally.map(([, v]) => v), backgroundColor: '#7a5cff' }],
      },
      options: { ...chartTheme(), indexAxis: 'y' },
    });

    // Distinct events per sport (sport -> set of event names)
    const eventMap = new Map();
    for (const r of rows) {
      if (!r.sport || !r.event) continue;
      if (!eventMap.has(r.sport)) eventMap.set(r.sport, new Set());
      eventMap.get(r.sport).add(r.event);
    }
    const eventTally = [...eventMap.entries()]
      .map(([s, set]) => [s, set.size])
      .sort((a, b) => b[1] - a[1])
      .slice(0, 15);
    new Chart(document.getElementById('chart-events'), {
      type: 'bar',
      data: {
        labels: eventTally.map(([k]) => k),
        datasets: [{ label: 'Distinct events', data: eventTally.map(([, v]) => v), backgroundColor: '#3cd4a8' }],
      },
      options: { ...chartTheme(), indexAxis: 'y' },
    });
  }

  // ---------- Tabs ----------

  function setupTabs() {
    const tabs = document.querySelectorAll('.tab');
    tabs.forEach(t => {
      t.addEventListener('click', () => {
        tabs.forEach(x => x.classList.remove('active'));
        document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
        t.classList.add('active');
        document.getElementById('tab-' + t.dataset.tab).classList.add('active');
      });
    });
  }

  // ---------- Boot ----------

  async function boot() {
    setupTabs();
    document.getElementById('search').addEventListener('input', (e) => {
      state.search = e.target.value;
      applyFilters();
    });
    document.getElementById('medal-filter').addEventListener('change', (e) => {
      state.medal = e.target.value;
      applyFilters();
    });
    document.getElementById('prev-page').addEventListener('click', () => {
      if (state.page > 0) { state.page--; renderTable(); }
    });
    document.getElementById('next-page').addEventListener('click', () => {
      const total = Math.ceil(state.filtered.length / PAGE_SIZE);
      if (state.page < total - 1) { state.page++; renderTable(); }
    });

    let text;
    try {
      const res = await fetch('data/athletes.csv');
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      text = await res.text();
    } catch (err) {
      document.getElementById('loading').textContent =
        `Failed to load data/athletes.csv (${err.message}). If you opened this file directly, run a local server (e.g. \`python3 -m http.server\`) from the docs/ folder.`;
      return;
    }

    const raw = parseCSV(text);
    state.rows = buildRows(raw);
    state.filtered = state.rows;

    renderKPIs(state.rows);
    renderTable();
    renderCountriesChart(state.rows);
    renderMedalsStackChart(state.rows);
    renderAthletesChart(state.rows);
    renderSportsCharts(state.rows);

    document.getElementById('loading').classList.add('hidden');
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', boot);
  } else {
    boot();
  }
})();
