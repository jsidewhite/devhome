﻿// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;
using DevHome.Common.Extensions;
using DevHome.Common.Services;
using DevHome.Common.TelemetryEvents;
using DevHome.Common.TelemetryEvents.SetupFlow;
using DevHome.SetupFlow.Common.Helpers;
using DevHome.SetupFlow.Models;
using DevHome.SetupFlow.Services;
using DevHome.SetupFlow.TaskGroups;
using DevHome.SetupFlow.Utilities;
using DevHome.Telemetry;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Windows.Storage;
using Windows.System;

namespace DevHome.SetupFlow.ViewModels;

/// <summary>
/// View model for the main page of the Setup Flow.
/// This page contains controls to start the setup flow with different
/// combinations of steps to perform. For example, only Configuration file,
/// or a full flow with Dev Volume, Clone Repos, and App Management.
/// </summary>
public partial class MainPageViewModel : SetupPageViewModelBase
{
    private const string _hideSetupFlowBannerKey = "HideSetupFlowBanner";

    private readonly IHost _host;
    private readonly IWindowsPackageManager _wpm;

    [ObservableProperty]
    private bool _showBanner = true;

    [ObservableProperty]
    private bool _showDevDriveItem;

    [ObservableProperty]
    private bool _enablePackageInstallerItem;

    [ObservableProperty]
    private bool _showAppInstallerUpdateNotification;

    /// <summary>
    /// Event raised when the user elects to start the setup flow.
    /// The orchestrator for the whole flow subscribes to this event to handle
    /// all the work needed at that point.
    /// </summary>
    public event EventHandler<(string, IList<ISetupTaskGroup>)> StartSetupFlow;

    public MainPageViewModel(
        ISetupFlowStringResource stringResource,
        SetupFlowOrchestrator orchestrator,
        IWindowsPackageManager wpm,
        IHost host)
        : base(stringResource, orchestrator)
    {
        _host = host;
        _wpm = wpm;

        IsNavigationBarVisible = false;
        IsStepPage = false;
        ShowDevDriveItem = DevDriveUtil.IsDevDriveFeatureEnabled;
        ShowBanner = ShouldShowSetupFlowBanner();
    }

    protected async override Task OnFirstNavigateToAsync()
    {
        EnablePackageInstallerItem = await _wpm.IsCOMServerAvailableAsync();
        if (EnablePackageInstallerItem)
        {
            Log.Logger?.ReportInfo($"{nameof(WindowsPackageManager)} COM Server is available. Showing package install item");
            ShowAppInstallerUpdateNotification = await _wpm.IsAppInstallerUpdateAvailableAsync();
        }
        else
        {
            Log.Logger?.ReportWarn($"{nameof(WindowsPackageManager)} COM Server is not available. Package install item is hidden.");
        }
    }

    [RelayCommand]
    private void HideBanner()
    {
        TelemetryFactory.Get<ITelemetry>().LogCritical("MainPage_HideLearnMoreBanner_Event", false, Orchestrator.ActivityId);
        var roamingProperties = ApplicationData.Current.RoamingSettings.Values;
        roamingProperties[_hideSetupFlowBannerKey] = bool.TrueString;
        ShowBanner = false;
    }

    /// <summary>
    /// Starts the setup flow including the pages for the given task groups.
    /// </summary>
    /// <param name="flowTitle">Title to show in the flow; will use the SetupShell.Title property if empty</param>
    /// <param name="taskGroups">The task groups that will be included in this setup flow.</param>
    /// <remarks>
    /// Note that the order of the task groups here will influence the order of the pages in
    /// the flow and the tabs in the review page.
    /// </remarks>
    private void StartSetupFlowForTaskGroups(string flowTitle, string flowNameForTelemetry, params ISetupTaskGroup[] taskGroups)
    {
        StartSetupFlow.Invoke(null, (flowTitle, taskGroups));

        // Report this after setting the flow pages as that will set an ActivityId
        // we can later use to correlate with the flow termination.
        Log.Logger?.ReportInfo($"Starting setup flow with ActivityId={Orchestrator.ActivityId}");
        TelemetryFactory.Get<ITelemetry>().Log(
            "MainPage_StartFlow_Event",
            LogLevel.Critical,
            new StartFlowEvent(flowNameForTelemetry),
            relatedActivityId: Orchestrator.ActivityId);
    }

    /// <summary>
    /// Starts a full setup flow, with all the possible task groups.
    /// </summary>
    [RelayCommand]
    private void StartSetup(string flowTitle)
    {
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Starting end-to-end setup");
        StartSetupFlowForTaskGroups(
            flowTitle,
            "EndToEnd",
            _host.GetService<RepoConfigTaskGroup>(),
            _host.GetService<AppManagementTaskGroup>(),
            _host.GetService<DevDriveTaskGroup>());
    }

    /// <summary>
    /// Starts a setup flow that only includes repo config.
    /// </summary>
    [RelayCommand]
    private void StartRepoConfig(string flowTitle)
    {
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Starting flow for repo config");
        StartSetupFlowForTaskGroups(
            flowTitle,
            "RepoConfig",
            _host.GetService<RepoConfigTaskGroup>(),
            _host.GetService<DevDriveTaskGroup>());
    }

    /// <summary>
    /// Starts a setup flow that only includes app management.
    /// </summary>
    [RelayCommand]
    private void StartAppManagement(string flowTitle)
    {
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Starting flow for app management");
        StartSetupFlowForTaskGroups(flowTitle, "AppManagement", _host.GetService<AppManagementTaskGroup>());
    }

    /// <summary>
    /// Opens the Windows settings app and redirects the user to the disks and volumes page.
    /// </summary>
    [RelayCommand]
    private async Task LaunchDisksAndVolumesSettingsPageAsync()
    {
        // Critical level approved by subhasan
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Launching settings on Disks and Volumes page");
        TelemetryFactory.Get<ITelemetry>().Log(
            "LaunchDisksAndVolumesSettingsPageTriggered",
            LogLevel.Critical,
            new DisksAndVolumesSettingsPageTriggeredEvent(source: "MainPageView"),
            Orchestrator.ActivityId);

        await Launcher.LaunchUriAsync(new Uri("ms-settings:disksandvolumes"));
    }

    /// <summary>
    /// Starts a setup flow that only includes configuration file.
    /// </summary>
    [RelayCommand]
    private async Task StartConfigurationFileAsync()
    {
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Launching configuration file flow");
        var configFileSetupFlow = _host.GetService<ConfigurationFileTaskGroup>();
        if (await configFileSetupFlow.PickConfigurationFileAsync())
        {
            Log.Logger?.ReportInfo(Log.Component.MainPage, "Starting flow for Configuration file");
            StartSetupFlowForTaskGroups(null, "ConfigurationFile", configFileSetupFlow);
        }
    }

    [RelayCommand]
    private void StartDevQuietMode(string flowTitle)
    {
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Navigating to Developer Quiet Mode page");

        // MainFrame.Navigate(typeof(NewPage));
        // MainPageViewModel.Navigate(typeof(DeveloperQuietModeView));
        // var navigationService = _host.Services.GetService<INavigationService>();
        var navigationService = _host.GetService<INavigationService>();

        // navigationService.NavigateTo(typeof(DevHome.SetupFlow.ViewModels.SetupFlowViewModel).FullName!);
        navigationService.NavigateTo(typeof(DevHome.SetupFlow.ViewModels.DeveloperQuietModeViewModel).FullName!);
    }

    [RelayCommand]
    private async Task BannerButtonAsync()
    {
        await Launcher.LaunchUriAsync(new ("https://go.microsoft.com/fwlink/?linkid=2235076"));
    }

    [RelayCommand]
    private void HideAppInstallerUpdateNotification()
    {
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Hiding AppInstaller update notification");
        ShowAppInstallerUpdateNotification = false;
    }

    [RelayCommand]
    private async Task UpdateAppInstallerAsync()
    {
        HideAppInstallerUpdateNotification();
        Log.Logger?.ReportInfo(Log.Component.MainPage, "Opening AppInstaller in the Store app");
        await Launcher.LaunchUriAsync(new Uri($"ms-windows-store://pdp/?productid={WindowsPackageManager.AppInstallerProductId}"));
    }

    private bool ShouldShowSetupFlowBanner()
    {
        var roamingProperties = ApplicationData.Current.RoamingSettings.Values;
        return !roamingProperties.ContainsKey(_hideSetupFlowBannerKey);
    }
}
