// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Net.WebSockets;
using System.Runtime.InteropServices.WindowsRuntime;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Documents;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;

namespace DevHome.QuietBackgroundProcesses.CustomControl.Table;

public sealed class TableCustomControl : Control
{
    private static readonly DependencyProperty LabelProperty = DependencyProperty.Register(
            nameof(Label),
            typeof(string),
            typeof(TableCustomControl),
            new PropertyMetadata(default));

    public static readonly DependencyProperty ItemsSourceProperty = DependencyProperty.Register(
        nameof(ItemsSource),
        typeof(object),
        typeof(TableCustomControl),
        new PropertyMetadata(default));

    public TableCustomControl()
    {
        this.DefaultStyleKey = typeof(TableCustomControl);


        List<string> Projects = new List<string>();

        var sdf = "string";
        Projects.Add(sdf);

        cvsProjects.Source = Projects;
    }

    public string Label
    {
        get => (string)GetValue(LabelProperty);
        set => SetValue(LabelProperty, value);
    }

    public bool HasLabelValue { get; set; }

    private static void OnLabelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        if (d is TableCustomControl labelControl)
        {
            labelControl.HasLabelValue = !(e.NewValue?.ToString() == string.Empty);
        }
    }

    public object ItemsSource
    {
        get => (object)GetValue(ItemsSourceProperty);
        set => SetValue(ItemsSourceProperty, value);
    }
}
