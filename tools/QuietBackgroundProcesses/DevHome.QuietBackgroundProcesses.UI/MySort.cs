// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommunityToolkit.WinUI.Collections;

namespace DevHome.QuietBackgroundProcesses.UI;

internal sealed class MySort : SortDescription
{
    private IComparer<object>? _comparer;

    public MySort(string propertyName, SortDirection direction, IComparer<object> comparer)
        : base(propertyName, direction)
    {
        _comparer = comparer;
    }
}
