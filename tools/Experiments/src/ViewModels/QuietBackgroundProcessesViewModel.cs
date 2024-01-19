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
    public string Name3 => "sdfsdfdsf34534";

    private bool _isExtensionEnabled = true;

    public bool IsExtensionEnabled
    {
        get => _isExtensionEnabled;

        set
        {
            if (_isExtensionEnabled != value)
            {
                _isExtensionEnabled = value;
                DoThings(_isExtensionEnabled);
            }
        }
    }

    private readonly int _timesToTick = 10;
    private DispatcherTimer _dispatcherTimer;
    private DateTimeOffset _startTime;
    private DateTimeOffset _lastTime;
    private DateTimeOffset _stopTime;
    private int _timesTicked = 1;

    public void DispatcherTimerSetup()
    {
        _dispatcherTimer = new DispatcherTimer();
        _dispatcherTimer.Tick += DispatcherTimer_Tick;
        _dispatcherTimer.Interval = new TimeSpan(0, 0, 1);

        _startTime = DateTimeOffset.Now;
        _lastTime = _startTime;
        _dispatcherTimer.Start();
    }

    private void DispatcherTimer_Tick(object sender, object e)
    {
        DateTimeOffset time = DateTimeOffset.Now;
        TimeSpan span = time - _lastTime;
        _lastTime = time;
        TimeLeft = span.ToString(); // CultureInfo.InvariantCulture
        _timesTicked++;
        if (_timesTicked > _timesToTick)
        {
            _stopTime = time;
            _dispatcherTimer.Stop();
        }
    }

    private string _timeLeft = "1:59:15";

    public string TimeLeft
    {
        get
        {
            return _timeLeft;
        }

        set
        {
            _timeLeft = value;

            // NotifyPropertyChanged();
            OnPropertyChanged(nameof(TimeLeft));
        }
    }

    public QuietBackgroundProcessesViewModel()
    {
    }

    private void DoThings(bool iee)
    {
        // TimeLeft = DateTime.Now.ToString();
        // TimeLeft = string.Create(CultureInfo.InvariantCulture, $"Hello {name}");
        // TimeLeft = DateTime.Now.ToString(CultureInfo.InvariantCulture);

        // TimeLeft = iee.ToString();
        DispatcherTimerSetup();
        Log.Logger()?.ReportInfo("PackageDeploymentService", $"Found package  IEEEEEEEEEEE {iee}");
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
