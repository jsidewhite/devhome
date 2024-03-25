// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using DevHome.Common;
using DevHome.QuietBackgroundProcesses.UI.ViewModels;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class QuietBackgroundProcessesPage : ToolPage
{
    public override string ShortName => "Quiet Background Processes";

    public QuietBackgroundProcessesViewModel ViewModel
    {
        get;
    }

    public QuietBackgroundProcessesPage()
    {
        ViewModel = new QuietBackgroundProcessesViewModel();
        InitializeComponent();
    }

    private async void ShowAnalyticSummaryButton_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        var analyticSummaryPopup = new AnalyticSummaryPopup();
        analyticSummaryPopup.XamlRoot = this.Content.XamlRoot;
        analyticSummaryPopup.RequestedTheme = this.ActualTheme;
        await analyticSummaryPopup.ShowAsync();
    }
}
