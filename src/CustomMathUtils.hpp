#pragma once

class CustomMathUtils
{
public:
    static double map(double x, double in_min, double in_max, double out_min, double out_max)
    {
        // Degenerate input range
        if (in_min == in_max) return (out_min + out_max) * 0.5;

        // Clamp input, respecting direction
        if (in_min < in_max) 
        {
            if (x <= in_min) return out_min;
            if (x >= in_max) return out_max;
        } else 
        { // reversed input range
            if (x >= in_min) return out_min;
            if (x <= in_max) return out_max;
        }

        // Linear mapping (works for both directions)
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }
};