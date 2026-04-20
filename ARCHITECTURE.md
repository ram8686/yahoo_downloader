# Yahoo Downloader – Architecture

## UI Layer
- MainDialog
  - Collects user input (ticker, index, interval, range/dates)
  - Launches ProgressDialog

- ProgressDialog
  - Controls download lifecycle
  - Starts worker thread
  - Handles cancel / completion
  - Displays progress + final summary

---

## Worker Layer
- DownloaderWorker
  - Runs in QThread
  - Processes tickers sequentially
  - Emits progress, status, finished

---

## Data Layer
- downloader.cpp
  - Builds URL
  - Fetches data (libcurl)

- JSON parsing
  - Extract OHLCV

---

## Utility Layer
- csv_writer
- file_utils
- config_manager
- fetch_nse_tickers

---

## Data Flow
UI → ProgressDialog → Worker → Downloader → CSV
                           ↓
                        Signals → UI

---

## Download Flow

1. User selects input in MainDialog
2. ProgressDialog is launched
3. startDownload():
   - builds ticker list
   - creates worker + thread
4. Worker::process():
   - loops tickers
   - fetch → parse → write
   - emits progress/status
5. On completion/cancel:
   - emits finished(...)
6. ProgressDialog:
   - receives result
   - shows summary dialog


---

## Signal / Slot Map

### UI → ProgressDialog

MainDialog
  └── (exec accepted)
        → creates ProgressDialog

---

### ProgressDialog → Worker

ProgressDialog::startDownload()
  └── QThread::started
        → DownloaderWorker::process()

---

### Worker → ProgressDialog

DownloaderWorker::progress(int)
  → ProgressDialog updates progress bar

DownloaderWorker::status(QString)
  → ProgressDialog updates status label

DownloaderWorker::finished(bool, failed, sec, total, completed)
  → ProgressDialog::showFinalSummary()

---

### User Actions → Worker

ProgressDialog cancel button
  → worker->requestStop()

Window close (❌)
  → confirmation
  → worker->requestStop()

---

### Thread Lifecycle

Worker::finished
  → QThread::quit()
  → deleteLater()