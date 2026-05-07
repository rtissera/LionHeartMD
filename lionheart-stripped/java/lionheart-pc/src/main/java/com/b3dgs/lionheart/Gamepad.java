/*
 * Copyright (C) 2013-2026 Byron 3D Games Studio (www.b3dgs.com) Pierre-Alexandre (contact@b3dgs.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */
package com.b3dgs.lionheart;

import java.util.Collections;
import java.util.Map;

import com.b3dgs.lionengine.InputDevice;
import com.b3dgs.lionengine.InputDeviceListener;

/**
 * Stub gamepad - lwjgl/GLFW dependency stripped for C/SDL2 porting reference.
 * Keyboard input (AWT) is the only path supported in this stripped build.
 */
public class Gamepad implements InputDevice
{
    public Gamepad()
    {
        // no-op
    }

    public Map<Integer, Integer> findDevices()
    {
        return Collections.emptyMap();
    }

    @Override
    public void update(double extrp)
    {
        // no-op
    }

    @Override
    public void addListener(InputDeviceListener listener)
    {
        // no-op
    }

    @Override
    public void removeListener(InputDeviceListener listener)
    {
        // no-op
    }

    @Override
    public String getName()
    {
        return "Gamepad(stub)";
    }
}
