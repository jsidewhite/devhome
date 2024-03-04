// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using System.Xml;
using DevHome.Common;
using DevHome.Common.Extensions;
using DevHome.Common.Helpers;
using DevHome.Common.Services;
using DevHome.QuietBackgroundProcesses.UI.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI.Xaml.Resources;
using Windows.Foundation;
using Windows.Foundation.Collections;

namespace DevHome.QuietBackgroundProcesses.UI.Views;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class QuietBackgroundProcessesPage : ToolPage
{
    public override string ShortName => "Quiet Background Processes";

    private bool _isFocusEnabled;

    public QuietBackgroundProcessesViewModel ViewModel
    {
        get;
    }

    public QuietBackgroundProcessesPage()
    {
        ViewModel = new QuietBackgroundProcessesViewModel();
        InitializeComponent();
        SetButtonVisibility();
    }

    private void SetButtonVisibility()
    {
        // Show Start/Stop focus session button exclusively
        if (_isFocusEnabled)
        {
            this.focusStartButton.Visibility = Visibility.Collapsed;
            this.focusStopButton.Visibility = Visibility.Visible;

            // Need to set focus back on focusStopButton or else the collapsed
            // focusStartButton will shift the focus away to the next element
            this.focusStopButton.Focus(FocusState.Programmatic);
        }
        else
        {
            this.focusStopButton.Visibility = Visibility.Collapsed;
            this.focusStartButton.Visibility = Visibility.Visible;

            // Need to set focus back on focusStopButton or else the collapsed
            // focusStartButton will shift the focus away to the next element
            this.focusStartButton.Focus(FocusState.Programmatic);
        }
    }

    private void OnFocusStartClicked(object sender, RoutedEventArgs e)
    {
        // Start the focus session
        _isFocusEnabled = true;
        SetButtonVisibility();
        ViewModel.StartStopSession(true);
    }

    private void OnFocusStopClicked(object sender, RoutedEventArgs e)
    {
        // Start the focus session
        _isFocusEnabled = false;
        SetButtonVisibility();
        ViewModel.StartStopSession(false);
    }
}
