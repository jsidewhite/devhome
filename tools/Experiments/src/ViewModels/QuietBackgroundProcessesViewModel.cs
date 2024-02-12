﻿// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using DevHome.Common;
using DevHome.Common.Helpers;
using Microsoft.UI.Xaml;
using Windows.UI.Xaml;
using Windows.Win32;

namespace DevHome.Experiments.ViewModels;
public class QuietBackgroundProcessesViewModel : INotifyPropertyChanged
{
    private readonly TimeSpan _zero;
    private readonly bool _isElevated;
    private readonly bool _validOsVersion;

    // [DllImport("ole32.dll", ExactSpelling = true, EntryPoint = "CoCreateInstance", PreserveSig = true)]
    // private static extern Result CoCreateInstance([In, MarshalAs(UnmanagedType.LPStruct)] Guid rclsid, IntPtr pUnkOuter, CLSCTX dwClsContext, [In, MarshalAs(UnmanagedType.LPStruct)] Guid riid, out IntPtr comObject);
    /*
    internal static void CreateComInstance(Guid clsid, CLSCTX clsctx, Guid riid, ComObject comObject)
    {
        IntPtr pointer;
        var result = CoCreateInstance(clsid, IntPtr.Zero, clsctx, riid, out pointer);
        result.CheckError();
        comObject.NativePointer = pointer;
    }
    */

    [DllImport("ole32.Dll")]
    private static extern int CoCreateInstance(
        ref Guid clsid,
        [MarshalAs(UnmanagedType.IUnknown)] object inner,
        int context,
        ref Guid uuid,
        [MarshalAs(UnmanagedType.IDispatch)] out object rReturnedComObject);

    public QuietBackgroundProcessesViewModel()
    {
        _zero = new TimeSpan(0, 0, 0);

        var osVersion = Environment.OSVersion;
        _validOsVersion = osVersion.Version.Build >= 26024;
        _validOsVersion = true;

        using (WindowsIdentity identity = WindowsIdentity.GetCurrent())
        {
            WindowsPrincipal principal = new WindowsPrincipal(identity);
            _isElevated = principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        unsafe
        {
            Guid the_CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe = new Guid("33a0a1a0-b89c-44af-ba17-c828cea010c2");
            Guid the_CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe1 = new Guid("13a0a1a0-b89c-44af-ba17-c828cea010c2");
            Guid the_IID_IUnknown = new Guid("00000000-0000-0000-C000-000000000046");
            const int CLSCTX_LOCAL_SERVER = 4;

            int hr = 0;
            object pIShellWindows;
            hr = CoCreateInstance(ref the_CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe, null, CLSCTX_LOCAL_SERVER, ref the_IID_IUnknown, out pIShellWindows);
            if (hr != 0)
            {
                // return false;
            }

            int hr1 = 0;
            object pIShellWindows1;
            hr1 = CoCreateInstance(ref the_CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe1, null, CLSCTX_LOCAL_SERVER, ref the_IID_IUnknown, out pIShellWindows1);
            if (hr1 != 0)
            {
                // return false;
            }
        }

        return;
         /*
        if (!_validOsVersion)
        {
            TimeLeft = "This feature requires OS version 10.0.26024.0+";
            return;
        }

        using (WindowsIdentity identity = WindowsIdentity.GetCurrent())
        {
            WindowsPrincipal principal = new WindowsPrincipal(identity);
            _isElevated = principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        _isElevated = true;

        // Resume countdown if there's an existing quiet window
        if (GetIsActive())
        {
            _isToggleOn = true;
            var timeLeftInSeconds = GetTimeRemaining();
            StartCountdownTimer(timeLeftInSeconds);
        }
        else
        {
            if (!_isElevated)
            {
                TimeLeft = "This feature requires running as admin";
            }
        }
         */
    }

    public bool IsToggleEnabled
    {
        get
        {
            CpuUsageCode = "IsToggleEnabled: ";
            return _isElevated && _validOsVersion;
        }
    }

    private bool _isToggleOn;

    public bool IsToggleOn
    {
        get => _isToggleOn;

        set
        {
            if (_isToggleOn == value)
            {
                return;
            }

            _isToggleOn = value;

            // Stop any existing timer
            _dispatcherTimer?.Stop();

            if (_isToggleOn)
            {
                try
                {
                    // var x = new DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.Get;
                    // var timeLeftInSeconds = DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.Start();
                    // StartCountdownTimer(timeLeftInSeconds);
                }
                catch
                {
                    TimeLeft = "Service error";
                }
            }
            else
            {
                try
                {
                    // DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.Stop();
                    TimeLeft = "Session ended";
                }
                catch
                {
                    TimeLeft = "Unable to cancel session";
                }
            }

            OnPropertyChanged(nameof(IsToggleOn));
        }
    }

    private bool GetIsActive()
    {
        try
        {
            // return DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.IsActive;
            return true;
        }
        catch (Exception ex)
        {
            TimeLeft = "Session error";
            Log.Logger()?.ReportInfo("QuietBackgroundProcesses", $"IsActive = {ex.ToString()}");
            return false;
        }
    }

    private int GetTimeRemaining()
    {
        try
        {
            // return (int)DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.TimeLeftInSeconds;
            return 0;
        }
        catch (Exception ex)
        {
            TimeLeft = "Session error";
            Log.Logger()?.ReportInfo("QuietBackgroundProcesses", $"TimeLeftInSeconds = {ex.ToString()}");
            return 0;
        }
    }

    private DispatcherTimer _dispatcherTimer;
    private TimeSpan _secondsLeft;

    private void StartCountdownTimer(long timeLeftInSeconds)
    {
        if (timeLeftInSeconds <= 0)
        {
            return;
        }

        _dispatcherTimer = new DispatcherTimer();
        _dispatcherTimer.Tick += DispatcherTimer_Tick;
        _dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
        _secondsLeft = new TimeSpan(0, 0, (int)timeLeftInSeconds);
        _dispatcherTimer.Start();

        TimeLeft = _secondsLeft.ToString();
    }

    private void DispatcherTimer_Tick(object sender, object e)
    {
        var sessionEnded = false;

        // CpuUsageCode = "18928: " + DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.GetProcessCpuUsage(21068);
        _secondsLeft = new TimeSpan(0, 0, GetTimeRemaining());

        if (_secondsLeft.CompareTo(_zero) <= 0)
        {
            // The window should be closed, but let's confirm with the server
            if (GetIsActive())
            {
                // There has been some drift
                _secondsLeft = new TimeSpan(0, 0, GetTimeRemaining());
            }
            else
            {
                _dispatcherTimer.Stop();
                _secondsLeft = _zero;
                IsToggleOn = false;
                sessionEnded = true;
            }
        }

        if (sessionEnded)
        {
            TimeLeft = "Session ended";
        }
        else
        {
            TimeLeft = _secondsLeft.ToString(); // CultureInfo.InvariantCulture
        }
    }

    private string _timeLeft = string.Empty;

    public string TimeLeft
    {
        get => _timeLeft;

        set
        {
            _timeLeft = value;
            OnPropertyChanged(nameof(TimeLeft));
        }
    }

    // INotifyPropertyChanged members
    public event PropertyChangedEventHandler PropertyChanged;

    protected virtual void OnPropertyChanged(string propertyName)
    {
        var handler = PropertyChanged;
        if (handler != null)
        {
            handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    private string _cpuUsageCode = "werw";

    public string CpuUsageCode
    {
        get => _cpuUsageCode;

        set
        {
            _cpuUsageCode = value;
            OnPropertyChanged(nameof(CpuUsageCode));
        }
    }
}
