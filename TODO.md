# Yahoo Downloader – TODO

## 🔧 Immediate (Next Session)

- [X] Add proper application / dialog icon (Qt + Windows exe)
- [X] Verify "Dates" mode (instead of range)
- [X] Fix default date display in UI

---

## 🧠 Functional Improvements

- [X] Validate date input (invalid ranges, future dates, etc.)
- [ ] Handle empty / partial data responses gracefully
- [X] Improve error handling for failed downloads

---

## 🎨 UI / UX

- [X] Replace QMessageBox with proper summary dialog
- [ ] Scrollable list for failed tickers
- [ ] Optional: copy/export failed tickers
- [X] Show live "Completed: X / Y" during download

---

## ⚙️ Performance / Stability

- [ ] Ensure cancel is responsive during long network calls
- [ ] Review threading for edge cases

---

## 📦 Packaging

- [ ] Bundle required DLLs (Qt, libcurl, etc.)
- [ ] Create installer (preferred over manual copy)
- [ ] Test on clean system

---

## 🧹 Later Cleanup

- [ ] Remove debug prints / temporary logs
- [ ] Review config handling (settings.ini)
- [ ] Consider saving last used UI settings
- [ ] Remove global `completed` counter:
      - move to DownloaderWorker as a member variable
      - ensure it resets per run
      - avoid shared/global state (thread-safety risk)
- [ ] Move HTTP logic (`fetch_data`) to shared utility:
      - create network_utils.{h,cpp}
      - eliminate dependency on downloader in fetch_nse_tickers
- [ ] Extract OHLCV struct to a shared header (e.g., data_types.h):
      - remove dependency of csv_writer on downloader
      - keep downloader focused on logic, not shared data
- [ ] Make CSV format configurable:
      - support selectable format types (e.g., AmiBroker, simple)
      - optionally allow date/time format and precision via settings.ini
      - avoid fully dynamic template-based CSV generation
- [ ] Make output directory configurable:
      - move base paths (YahooIntraday / YahooEOD) to settings.ini
      - allow custom output root directory

---

## 💡 Future Ideas

- [ ] Retry failed tickers
- [ ] Export to different formats (CSV variations)
- [ ] Add logging file