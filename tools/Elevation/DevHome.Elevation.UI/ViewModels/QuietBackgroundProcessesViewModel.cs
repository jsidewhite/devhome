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
using WinRTServer;
using WinUIEx;

namespace DevHome.Elevation.UI.ViewModels;

public partial class ElevationViewModel : ObservableObject
{
    private readonly ILogger _log = Log.ForContext("SourceContext", nameof(ElevationViewModel));

    [ObservableProperty]
    private bool _isFeaturePresent;

    [ObservableProperty]
    private string _sessionStateText;

    [ObservableProperty]
    private bool _quietButtonChecked;

    public ElevationViewModel(IExperimentationService experimentationService)
    {
        _sessionStateText = string.Empty;

        IsFeaturePresent = ElevationSessionManager.IsFeaturePresent();
        QuietButtonChecked = false;

        SessionStateText = "<unset>";
    }

    [RelayCommand]
    public void QuietButtonClicked()
    {
        if (IsFeaturePresent)
        {
            SessionStateText = ElevationSessionManager.GetInt.ToString(new CultureInfo("en-US"));
        }
        else
        {
            SessionStateText = "!IsFeaturePresent";
        }

        SessionStateText = CalcClass.Add(12, 34).ToString(new CultureInfo("en-US"));
    }
}
