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
using CommunityToolkit.Mvvm.ComponentModel;
using DevHome.Common;
using DevHome.Common.Helpers;
using DevHome.Common.Services;
using DevHome.QuietBackgroundProcesses;
using Microsoft.UI.Xaml;
using Microsoft.Windows.ApplicationModel.Resources;
using Windows.UI.Xaml;
using Windows.Win32;

namespace DevHome.QuietBackgroundProcesses.UI.ViewModels;

public partial class QuietBackgroundProcessesViewModel : ObservableObject
{
    private readonly bool _isFeaturePresent;
    private readonly TimeSpan _zero;
    private readonly TimeSpan _oneSecond;
#nullable enable
    private DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession? _session;
#nullable disable

    [ObservableProperty]
    private string _sessionStateText;

    [ObservableProperty]
    private string _quietButtonText;

    private enum ButtonState
    {
        Start,
        Stop,
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

    private string GetStatusString(string id)
    {
        return GetString("QuietBackgroundProcesses_Status_" + id);
    }

    public QuietBackgroundProcessesViewModel()
    {
        _zero = new TimeSpan(0, 0, 0);
        _oneSecond = new TimeSpan(0, 0, 1);

        QuietButtonText = GetString("QuietBackgroundProcesses_QuietButton_Start");

        _isFeaturePresent = DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.IsFeaturePresent();
        if (!_isFeaturePresent)
        {
            SessionStateText = GetStatusString("FeatureNotSupported");
            return;
        }

        // Resume countdown if there's an existing quiet window
        //
        // Note: GetIsActive() won't ever launch a UAC prompt, but GetTimeRemaining() will if no session is running - so be careful with call order
        var running = GetIsActive();
        SetQuietSessionRunningState(running);
    }

    public bool IsButtonEnabled => _isFeaturePresent;

    private ButtonState _buttonState;

    private void SetQuietSessionRunningState(bool running, long? timeLeftInSeconds = null)
    {
        if (running)
        {
            _buttonState = ButtonState.Stop;
            var seconds = timeLeftInSeconds ?? GetTimeRemaining();
            StartCountdownTimer(seconds);
            QuietButtonText = GetString("QuietBackgroundProcesses_QuietButton_Stop");
        }
        else
        {
            _buttonState = ButtonState.Start;
            _dispatcherTimer?.Stop();
            QuietButtonText = GetString("QuietBackgroundProcesses_QuietButton_Start");
        }
    }

    public void QuietButtonClicked()
    {
        switch (_buttonState)
        {
            case ButtonState.Start:
                {
                    try
                    {
                        // Launch the server, which then elevates itself, showing a UAC prompt
                        var timeLeftInSeconds = GetSession().Start();
                        SetQuietSessionRunningState(true, timeLeftInSeconds);
                    }
                    catch (Exception ex)
                    {
                        SessionStateText = GetStatusString("SessionError");
                        Log.Logger()?.ReportError("QuietBackgroundProcessesSession::Start failed", ex);
                    }
                }

                break;
            case ButtonState.Stop:
                {
                    try
                    {
                        GetSession().Stop();
                        SetQuietSessionRunningState(false);
                        SessionStateText = GetStatusString("SessionEnded");
                    }
                    catch (Exception ex)
                    {
                        SessionStateText = GetStatusString("UnableToCancelSession");
                        Log.Logger()?.ReportError("QuietBackgroundProcessesSession::Stop failed", ex);
                    }
                }

                break;
        }
    }

    private bool GetIsActive()
    {
        try
        {
            _session = DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.TryGetSession();
            if (_session != null)
            {
                return _session.IsActive;
            }
        }
        catch (Exception ex)
        {
            SessionStateText = GetStatusString("SessionError");
            Log.Logger()?.ReportError("QuietBackgroundProcessesSession::IsActive failed", ex);
        }

        return false;
    }

    private int GetTimeRemaining()
    {
        try
        {
            return (int)GetSession().TimeLeftInSeconds;
        }
        catch (Exception ex)
        {
            SessionStateText = GetStatusString("SessionError");
            Log.Logger()?.ReportError("QuietBackgroundProcessesSession::TimeLeftInSeconds failed", ex);
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

        SessionStateText = _secondsLeft.ToString();
    }

    private void DispatcherTimer_Tick(object sender, object e)
    {
        // Subtract 1 second
        _secondsLeft = _secondsLeft.Subtract(_oneSecond);

        // Every 30 seconds ask the server for the actual time remaining (to resolve any drift)
        if (_secondsLeft.Seconds % 30 == 0)
        {
            _secondsLeft = new TimeSpan(0, 0, GetTimeRemaining());
        }

        var sessionEnded = false;
        if (_secondsLeft.CompareTo(_zero) <= 0)
        {
            // The window should be closed, but let's confirm with the server
            if (!GetSession().IsActive)
            {
                sessionEnded = true;
            }
        }

        if (sessionEnded)
        {
            SetQuietSessionRunningState(false);
            _secondsLeft = _zero;
            SessionStateText = GetStatusString("SessionEnded");
        }
        else
        {
            SessionStateText = _secondsLeft.ToString(); // CultureInfo.InvariantCulture
        }
    }
}
