// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Globalization;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using CommunityToolkit.WinUI;
using DevHome.Common.Services;
using DevHome.Telemetry;
using Microsoft.UI.Xaml;
using Serilog;
using Windows.Foundation.Diagnostics;
using WinUIEx;

namespace DevHome.QuietBackgroundProcesses.UI.ViewModels;

public partial class QuietBackgroundProcessesViewModel : ObservableObject
{
    private readonly ILogger _log = Log.ForContext("SourceContext", nameof(QuietBackgroundProcessesViewModel));

    [ObservableProperty]
    private bool _isFeaturePresent;

    [ObservableProperty]
    private string _sessionStateText;

    [ObservableProperty]
    private bool _quietButtonChecked;

    public QuietBackgroundProcessesViewModel(IExperimentationService experimentationService)
    {
        _sessionStateText = string.Empty;

        IsFeaturePresent = QuietBackgroundProcessesSessionManager.IsFeaturePresent();
        QuietButtonChecked = false;

        SessionStateText = "<unset>";
    }

    [RelayCommand]
    public void QuietButtonClicked()
    {
        if (IsFeaturePresent)
        {
            SessionStateText = QuietBackgroundProcessesSessionManager.GetInt.ToString(new CultureInfo("en-US"));
        }
        else
        {
            SessionStateText = "!IsFeaturePresent";
        }
    }
}
