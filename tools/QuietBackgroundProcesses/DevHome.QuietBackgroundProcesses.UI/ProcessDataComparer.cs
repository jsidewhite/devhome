// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DevHome.QuietBackgroundProcesses.UI;

internal sealed class ProcessDataComparer : IComparer<object>
{
    public int Compare(object? x, object? y)
    {
        if (x == null)
        {
            return y == null ? 0 : -1;
        }
        else if (y == null)
        {
            return 1;
        }

        var left = x as ProcessData;
        var right = y as ProcessData;

        return string.Compare(left?.Process, right?.Process, StringComparison.Ordinal);
    }
}
