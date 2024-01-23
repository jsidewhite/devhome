// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;
using DevHome.Common.Helpers;
using Microsoft.UI.Xaml;
using Windows.UI.Xaml;

namespace DevHome.Experiments.ViewModels;
public class QuietBackgroundProcessesViewModel : INotifyPropertyChanged
{
    public QuietBackgroundProcessesViewModel()
    {
        // Resume countdown if there's an existing quiet window
        if (QuietBackgroundProcesses_ElevatedServer.QuietWindow.IsActive)
        {
            var timeLeftInSeconds = QuietBackgroundProcesses_ElevatedServer.QuietWindow.TimeLeftInSeconds;
            StartCountdownTimer(timeLeftInSeconds);
        }
    }

    private void StartCountdownTimer(long timeLeftInSeconds)
    {
        DispatcherTimer_StartCountdown(timeLeftInSeconds);
    }

    private bool _isToggleOn;

    public bool IsToggleOn
    {
        get => _isToggleOn;

        set
        {
            if (_isToggleOn != value)
            {
                _isToggleOn = value;

                // Stop any existing timer
                _dispatcherTimer?.Stop();

                if (_isToggleOn)
                {
                    TimeLeft = "Starting...";
                    var timeLeftInSeconds = QuietBackgroundProcesses_ElevatedServer.QuietWindow.StartQuietWindow();
                    StartCountdownTimer(timeLeftInSeconds);
                }
                else
                {
                    QuietBackgroundProcesses_ElevatedServer.QuietWindow.StopQuietWindow();
                    TimeLeft = "Session ended";
                }

                OnPropertyChanged(nameof(IsToggleOn));
            }
        }
    }

    private DispatcherTimer _dispatcherTimer;
    private TimeSpan _secondsLeft;

    public void DispatcherTimer_StartCountdown(long timeLeftInSeconds)
    {
        _dispatcherTimer = new DispatcherTimer();
        _dispatcherTimer.Tick += DispatcherTimer_Tick;
        _dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
        _secondsLeft = new TimeSpan(0, 0, (int)timeLeftInSeconds);
        _dispatcherTimer.Start();
    }

    private void DispatcherTimer_Tick(object sender, object e)
    {
        var sessionEnded = false;
        var zero = new TimeSpan(0, 0, 0);

        // _secondsLeft -= new TimeSpan(0, 0, 1);
        var timeLeftInSeconds2 = QuietBackgroundProcesses_ElevatedServer.QuietWindow.TimeLeftInSeconds;
        _secondsLeft = new TimeSpan(0, 0, (int)timeLeftInSeconds2);

        if (_secondsLeft.CompareTo(zero) <= 0)
        {
            // The window should be closed, but let's confirm with the server
            if (QuietBackgroundProcesses_ElevatedServer.QuietWindow.IsActive)
            {
                // There has been some drift
                var timeLeftInSeconds = QuietBackgroundProcesses_ElevatedServer.QuietWindow.TimeLeftInSeconds;
                _secondsLeft = new TimeSpan(0, 0, (int)timeLeftInSeconds);
            }
            else
            {
                _dispatcherTimer.Stop();
                _secondsLeft = zero;
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
}
