// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DevHome.Common.Helpers;

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
        // TimeLeft = "1:11:11";
        TimeLeft = iee.ToString();
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
