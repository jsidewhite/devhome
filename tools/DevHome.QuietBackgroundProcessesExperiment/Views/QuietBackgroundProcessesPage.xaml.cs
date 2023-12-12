// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using DevHome.Common;
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

    public QuietBackgroundProcessesViewModel ViewModel
    {
        get;
    }

    public QuietBackgroundProcessesPage()
    {
        ViewModel = new QuietBackgroundProcessesViewModel();
        InitializeComponent();
    }
}
