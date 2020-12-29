How to enable WPP tracing.
  1. Launch Admin Command Prompt
  2. Execute following commands.
       logman create trace "sample_trace" -o D:\git\masarun\test\COMTest\Dll1\Log.etl -p {49B405DA-4FDB-43D5-A13B-A809180EAE58} 0xffffffff 0xff
       logman start "sample_trace"
       [Do some operations]
       logman stop "sample_trace"

How to dump debug log to a text file.
  1. Get Dll1.pdb and Log_xxxxxx.etl.
  2. Launch VS command prompt.
  3. Execute following commands.
       tracepdb -f Dll1.pdb
       tracefmt Log_xxxxxx.etl -p . -o output.txt
  4. output.txt contains TraceEvents text data.

How to read TraceEvents at runtime.
  1. Launch Traceview.exe.
  2. [File] - [Create New Log Session]
  3. Choose [PDB] and set Dll1.pdb.
  4. Launch ConsoleApplication1.exe.
  5. "DLL Loaded" message is displayed at the bottom half of the Window.

Reference:
  https://monoist.atmarkit.co.jp/mn/articles/1310/10/news001_2.html
  https://docs.microsoft.com/en-us/windows-hardware/drivers/devtest/adding-wpp-software-tracing-to-a-windows-driver#step-1-define-the-control-guid-and-trace-flags
  https://social.msdn.microsoft.com/Forums/WINDOWS/en-US/916de0ed-f194-4a87-9ae0-b2f3ed0fd56e/tracepdb-wppfmt-no-trace-entries-found-sample-device-mft?forum=wdk

