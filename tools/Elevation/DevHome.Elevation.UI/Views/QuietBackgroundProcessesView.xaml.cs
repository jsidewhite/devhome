// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using DevHome.Common.Extensions;
using DevHome.Elevation.UI.ViewModels;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace DevHome.Elevation.UI.Views;

public sealed partial class ElevationView : UserControl
{
    public ElevationViewModel ViewModel
    {
        get;
    }

    public ElevationView()
    {
        InitializeComponent();

        ViewModel = Application.Current.GetService<ElevationViewModel>();
    }
}
