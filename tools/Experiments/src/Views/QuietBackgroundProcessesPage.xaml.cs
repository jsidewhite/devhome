// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

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
using DevHome.Experiments.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Windows.Foundation;
using Windows.Foundation.Collections;

namespace DevHome.Experiments.Views;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class QuietBackgroundProcessesPage : ToolPage
{
    public override string ShortName => "Quiet Background Processes";

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

    public QuietBackgroundProcessesViewModel ViewModel
    {
        get;
    }

    public QuietBackgroundProcessesPage()
    {
        ViewModel = new QuietBackgroundProcessesViewModel();
        InitializeComponent();
    }

    private void StartSessionButton_Clicked(object sender, RoutedEventArgs e)
    {
        var x = new QuietBackgroundProcesses_ElevatedServer.Class();
        Log.Logger()?.ReportInfo("PackageDeploymentService", $"Found package {x.MyProperty}");
    }

    private void DoThings(bool iee)
    {
        Log.Logger()?.ReportInfo("PackageDeploymentService", $"Found package  IEEEEEEEEEEE {iee}");
    }
}
