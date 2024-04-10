// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using DevHome.Common.Extensions;
using DevHome.QuietBackgroundProcesses.UI.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

public sealed partial class QuietBackgroundProcessesView : UserControl
{
    public QuietBackgroundProcessesViewModel ViewModel
    {
        get;
    }

    public QuietBackgroundProcessesView()
    {
        InitializeComponent();

        ViewModel = Application.Current.GetService<QuietBackgroundProcessesViewModel>();
    }

    private async void ShowAnalyticSummaryButton_Click(object sender, Microsoft.UI.Xaml.RoutedEventArgs e)
    {
        var analyticSummaryPopup = new AnalyticSummaryPopup(ViewModel.GetProcessPerformanceTable());
        analyticSummaryPopup.XamlRoot = this.Content.XamlRoot;
        analyticSummaryPopup.RequestedTheme = this.ActualTheme;
        await analyticSummaryPopup.ShowAsync();
    }
}
