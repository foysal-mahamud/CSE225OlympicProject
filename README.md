# Paris 2024 Olympic Athlete Tracking System

A C++17 command-line application for managing athlete records from the Paris 2024 Summer Olympics. Records are stored in a custom singly linked list, persisted to CSV, and gated behind a simple file-backed authentication system. Built as a Data Structures & Algorithms (CSE225) course project at North South University.

> **Status:** Coursework project, refactored into a modular, cross-platform codebase. Builds on Windows, macOS, and Linux. Companion web dashboard auto-deploys to GitHub Pages.

🔗 **Live dashboard:** https://foysal-mahamud.github.io/CSE225OlympicProject/

---

## Features

- **Authentication** — register / login flow with masked password input, persisted to `users.dat`.
- **CRUD on athlete records** — add, view, search, update, delete, with all changes mirrored back to the CSV.
- **Paginated viewer** — page through records 5 at a time, or jump to a specific index.
- **Two search modes** — unique-key search (ID, name, medal ID) and common-field search (country, gender, sport, etc.).
- **Statistics reports**
  - Per-country medal breakdown (gold / silver / bronze).
  - Total unique participants.
  - Top 5 countries and top 5 athletes by medal count.
  - Most popular sports plus per-event medal counts.
- **Change tracking** — records modified during the session can be replayed at any time.
- **Cross-platform terminal handling** — masked password input works on Windows (`<conio.h>`) and POSIX (`<termios.h>`).

---

## Tech Stack & DSA Concepts

- **Language:** C++17 (standard library only, no external dependencies).
- **Data structures used:**
  - Singly linked list — the canonical store for athlete records.
  - `std::unordered_map` — user credential lookup and medal-count aggregation for stats.
  - `std::set` — change-index tracking.
  - `std::vector` + `std::sort` with lambda comparators — top-N rankings.
- **File I/O:** RFC 4180-compliant CSV parsing (handles quoted fields with embedded commas) and rewriting via `<fstream>` / `<sstream>`.
- **Platform abstraction:** isolated in a single `Platform` module.

---

## Project Structure

```
CSE225OlympicProject/
├── include/              # Public headers
│   ├── Athlete.h
│   ├── AthleteStore.h
│   ├── Auth.h
│   ├── CSVStore.h
│   ├── Menu.h
│   ├── Platform.h
│   └── Stats.h
├── src/                  # Implementations
│   ├── AthleteStore.cpp
│   ├── Auth.cpp
│   ├── CSVStore.cpp
│   ├── Menu.cpp
│   ├── Platform.cpp
│   ├── Stats.cpp
│   └── main.cpp
├── data/
│   └── athletes.csv      # Paris 2024 athlete data (30-column source format)
├── docs/                 # Static web dashboard (deployed to GitHub Pages)
│   ├── index.html
│   ├── styles.css
│   ├── app.js
│   └── data/athletes.csv # Mirror of the root data file
├── CMakeLists.txt
├── Makefile
├── LICENSE
└── README.md
```

---

## Build & Run

The program reads `data/athletes.csv` and writes `users.dat` relative to the **current working directory**, so always launch it from the project root.

### Option 1 — CMake (recommended)

```bash
cmake -S . -B build
cmake --build build
./build/olympic
```

### Option 2 — Make (POSIX)

```bash
make
./olympic
```

### Option 3 — Direct compile

```bash
g++ -std=c++17 -Iinclude src/*.cpp -o olympic
./olympic
```

### Windows (MSVC)

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\olympic.exe
```

---

## Usage

On first launch:

1. Choose **2. Register**, enter an email and password. Credentials are stored in `users.dat` in the working directory.
2. Choose **1. Login** with the same credentials.
3. Use the main menu (options 1–11) to browse, mutate, or summarize the data.

### Main menu

| Option | Action |
|-------:|--------|
| 1  | View records (all, paginated, or by index) |
| 2  | Search records (unique or common fields) |
| 3  | Add a new record |
| 4  | Update an existing record |
| 5  | Delete a record (by ID or index) |
| 6  | Show records changed this session |
| 7  | Per-country medal statistics |
| 8  | Total unique participants |
| 9  | Top 5 countries and athletes |
| 10 | Most popular sports & event medal counts |
| 11 | Exit |

---

## Data Schema

`data/athletes.csv` is sourced from the Paris 2024 official dataset and contains **30 columns**. The C++ application tracks the following 18 fields (blank/spacer columns in the source file are skipped on load):

| Source Column | Field | Description |
|--------------:|-------|-------------|
| 0 | Athlete ID | Unique numeric identifier |
| 1 | Name | Athlete full name |
| 2 | Gender | Male / Female |
| 3 | Type | e.g., Athlete, Coach |
| 5 | Country Name | Full country name |
| 6 | Native Language | Athlete's native language |
| 7 | Event Type | Sport category |
| 8 | Event Name | Specific event |
| 9 | Date of Birth | DD/MM/YYYY |
| 12 | Medal ID | Unique medal record ID |
| 13 | Medal Type | Gold Medal / Silver Medal / Bronze Medal |
| 14 | Medal Rank | 1 / 2 / 3 |
| 16 | Opponent Name | Name of opposing athlete (where applicable) |
| 18 | Game | e.g., Cycling Road |
| 23 | Start Time | ISO 8601 event start timestamp |
| 24 | End Time | ISO 8601 event end timestamp |
| 28 | Venue | Venue name |
| 29 | Stadium | Full stadium name and city |

> **Note:** After any save operation the program rewrites `data/athletes.csv` in a compact 18-column format. The web dashboard in `docs/` always reads its own copy (`docs/data/athletes.csv`) and is unaffected.

---

## Web Dashboard

A companion static dashboard lives in [`docs/`](docs/). To enable the live URL, go to **Repo Settings → Pages → Build and deployment → Source: Deploy from a branch → `main` / `/docs`** — GitHub will serve the dashboard at the URL shown at the top of this file.

Features:
- Searchable, paginated athlete table with medal filter.
- Top countries by medals (with gold / silver / bronze breakdown).
- Top 20 athletes by medal count.
- Most popular sports and event counts per sport.

To run it locally:

```bash
cd docs
python3 -m http.server 8000
# open http://localhost:8000
```

Tech: vanilla HTML + CSS + JS, Chart.js loaded from CDN, zero build step.

---

## Authors

CSE225 group project — North South University.

- Rafiur Rahman Mashrafi (ID: 2221971042)
- Dhruba Saha (ID: 2232537042)
- Md. Tanvir Rahman (ID: 2211462042)
- **Foysal Mahamud** (ID: 2231078042) — repository maintainer

---

## License

Released under the [MIT License](LICENSE).
