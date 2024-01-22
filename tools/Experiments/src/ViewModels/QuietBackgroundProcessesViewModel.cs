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
            StartCountdownTimer();
        }
    }

    private void StartCountdownTimer()
    {
        var timeLeftInSeconds = QuietBackgroundProcesses_ElevatedServer.QuietWindow.TimeLeftInSeconds;
        DispatcherTimer_StartCountdown(timeLeftInSeconds);
    }

    private bool _isToggleOn = true;

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
                    StartCountdownTimer();
                }
            }
        }
    }

    private DispatcherTimer _dispatcherTimer;
    private TimeSpan _secondsLeft;

    public void DispatcherTimer_StartCountdown(int timeLeftInSeconds)
    {
        _dispatcherTimer = new DispatcherTimer();
        _dispatcherTimer.Tick += DispatcherTimer_Tick;
        _dispatcherTimer.Interval = new TimeSpan(0, 0, 1);
        _secondsLeft = new TimeSpan(0, 0, timeLeftInSeconds);
        _dispatcherTimer.Start();
    }

    private void DispatcherTimer_Tick(object sender, object e)
    {
        _secondsLeft -= new TimeSpan(0, 0, 1);

        if (_secondsLeft <= new TimeSpan(0, 0, 0))
        {
            _dispatcherTimer.Stop();
        }

        TimeLeft = _secondsLeft.ToString(); // CultureInfo.InvariantCulture
    }

    private string _timeLeft = "1:59:15";

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
