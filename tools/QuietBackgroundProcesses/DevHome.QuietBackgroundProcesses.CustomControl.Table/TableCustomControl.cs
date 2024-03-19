// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
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
    private readonly DependencyProperty _labelProperty;

    public TableCustomControl()
    {
        this.DefaultStyleKey = typeof(TableCustomControl);

        _labelProperty = DependencyProperty.Register(
            nameof(Label),
            typeof(string),
            typeof(TableCustomControl),
            new PropertyMetadata(default(string), new PropertyChangedCallback(OnLabelChanged)));
    }

    public string Label
    {
        get => (string)GetValue(_labelProperty);
        set => SetValue(_labelProperty, value);
    }

    public bool HasLabelValue { get; set; }

    private static void OnLabelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        TableCustomControl labelControl = d as TableCustomControl;
        var s = e.NewValue as string;
        if (s == string.Empty)
        {
            labelControl.HasLabelValue = false;
        }
        else
        {
            labelControl.HasLabelValue = true;
        }
    }
}
