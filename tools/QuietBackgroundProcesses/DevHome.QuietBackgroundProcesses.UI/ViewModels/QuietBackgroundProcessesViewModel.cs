// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using DevHome.Common;
using DevHome.Common.Helpers;
using DevHome.Common.Services;
using DevHome.QuietBackgroundProcesses;
using Microsoft.UI.Xaml;
using Microsoft.Windows.ApplicationModel.Resources;
using Windows.UI.Xaml;
using Windows.Win32;

namespace DevHome.QuietBackgroundProcesses.UI.ViewModels;

public class QuietBackgroundProcessesViewModel : INotifyPropertyChanged
{
    private readonly bool _isFeatureSupported;
    private readonly TimeSpan _zero;
#nullable enable
    private DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession? _session;
#nullable disable

    private bool IsQuietModeServerRunning()
    {
        try
        {
            _session = DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.TryGetSession();
            return _session != null;
        }
        catch (Exception ex)
        {
            Log.Logger()?.ReportError("QuietBackgroundProcessesSessionManager::TryGetSession failed", ex);
        }

        return false;
    }

    private DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession GetSession()
    {
        if (_session == null)
        {
            _session = QuietBackgroundProcessesSessionManager.GetSession();
        }

        return _session;
    }

    private string GetString(string id)
    {
        var stringResource = new StringResource("DevHome.QuietBackgroundProcesses.UI/Resources");
        return stringResource.GetLocalized(id);
    }

    public QuietBackgroundProcessesViewModel()
    {
        _zero = new TimeSpan(0, 0, 0);

        TimeLeft = GetString("QuietBackgroundProcesses_Status_FeatureNotSupported");

        _isFeatureSupported = DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.IsFeatureSupported();
        if (!_isFeatureSupported)
        {
            // TimeLeft = "Feature not supported on this OS";
            return;
        }

        if (!IsQuietModeServerRunning())
        {
            // Make sure not to launch the elevated server (which shows UAC) until the user hits the Start button
            return;
        }

        // Resume countdown if there's an existing quiet window
        if (GetIsActive())
        {
            _isToggleOn = true;
            var timeLeftInSeconds = GetTimeRemaining();
            StartCountdownTimer(timeLeftInSeconds);
        }
    }

    public bool IsToggleEnabled
    {
        get
        {
            CpuUsageCode = "IsToggleEnabled: ";
            return _isFeatureSupported;
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
                    // Launch the server, which then elevates itself, showing a UAC prompt
                    var timeLeftInSeconds = GetSession().Start();
                    StartCountdownTimer(timeLeftInSeconds);
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
                    GetSession().Stop();
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
            return GetSession().IsActive;
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
            return (int)GetSession().TimeLeftInSeconds;
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

        _secondsLeft = new TimeSpan(0, 0, GetTimeRemaining());

        if (_secondsLeft.CompareTo(_zero) <= 0)
        {
            // The window should be closed, but let's confirm with the server
            if (GetSession().IsActive)
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
