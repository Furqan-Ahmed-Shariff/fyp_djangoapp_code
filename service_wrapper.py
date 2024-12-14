import win32serviceutil
import win32service
import win32event
import servicemanager
import os
import sys


class MyPythonService(win32serviceutil.ServiceFramework):
    _svc_name_ = "waitress_daemon"
    _svc_display_name_ = "waitress_runserver"
    _svc_description_ = "Runs my Python script as a service"

    def __init__(self, args):
        win32serviceutil.ServiceFramework.__init__(self, args)
        self.hWaitStop = win32event.CreateEvent(None, 0, 0, None)

    def SvcStop(self):
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)
        win32event.SetEvent(self.hWaitStop)

    def SvcDoRun(self):
        servicemanager.LogMsg(
            servicemanager.EVENTLOG_INFORMATION_TYPE,
            servicemanager.PYS_SERVICE_STARTED,
            (self._svc_name_, ""),
        )
        self.main()

    def main(self):
        # Execute your Python script
        script_path = r"D:\test\runserver.py"  # Modify this path to your script
        os.system(f"python {script_path}")


if __name__ == "__main__":
    win32serviceutil.HandleCommandLine(MyPythonService)
