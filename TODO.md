# Yahoo Downloader – TODO

## 🔧 Immediate (Next Session)

- [ ] Add proper application / dialog icon (Qt + Windows exe)
- [ ] Verify "Dates" mode (instead of range)
- [ ] Fix default date display in UI

---

## 🧠 Functional Improvements

- [ ] Validate date input (invalid ranges, future dates, etc.)
- [ ] Handle empty / partial data responses gracefully
- [ ] Improve error handling for failed downloads

---

## 🎨 UI / UX

- [ ] Replace QMessageBox with proper summary dialog
- [ ] Scrollable list for failed tickers
- [ ] Optional: copy/export failed tickers
- [ ] Show live "Completed: X / Y" during download

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

---

## 💡 Future Ideas

- [ ] Retry failed tickers
- [ ] Export to different formats (CSV variations)
- [ ] Add logging file