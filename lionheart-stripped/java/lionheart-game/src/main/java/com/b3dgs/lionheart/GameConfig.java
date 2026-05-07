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
import java.util.Optional;

import com.b3dgs.lionengine.SplitType;

/**
 * Game data configuration.
 *
 * Network strip applied: Optional<Network> field + getNetwork() removed for C/SDL2 port reference.
 */
public class GameConfig
{
    private final GameType type;
    private final int players;
    private final Optional<String> stages;
    private final Map<Integer, Integer> controls;
    private final InitConfig init;
    private final boolean oneButton;

    /**
     * Create first configuration.
     */
    public GameConfig()
    {
        this(GameType.STORY, 1, Optional.empty(), true, Collections.emptyMap(), null);
    }

    /**
     * Create configuration.
     *
     * @param type The game type.
     * @param players The players number.
     * @param stages The stages set.
     * @param oneButton <code>true</code> for one button mode, <code>false</code> two buttons.
     * @param controls The player id as key, the control index as value.
     * @param init The init configuration.
     */
    public GameConfig(GameType type,
                      int players,
                      Optional<String> stages,
                      boolean oneButton,
                      Map<Integer, Integer> controls,
                      InitConfig init)
    {
        super();

        this.type = type;
        this.players = players;
        this.stages = stages;
        this.controls = controls;
        this.init = init;
        this.oneButton = oneButton;
    }

    public GameConfig with(GameType type, int players, boolean oneButton, Map<Integer, Integer> controls)
    {
        return new GameConfig(type, players, stages, oneButton, controls, init);
    }

    public GameConfig with(String stages)
    {
        return new GameConfig(type, players, Optional.ofNullable(stages), oneButton, controls, init);
    }

    public GameConfig with(boolean oneButton)
    {
        return new GameConfig(type, players, stages, oneButton, controls, init);
    }

    public GameConfig with(InitConfig init)
    {
        return new GameConfig(type, players, stages, oneButton, controls, init);
    }

    public GameType getType()
    {
        return type;
    }

    public int getPlayers()
    {
        return players;
    }

    public Optional<String> getStages()
    {
        return stages;
    }

    public boolean isOneButton()
    {
        return oneButton;
    }

    public int getControl(int player)
    {
        return controls.get(Integer.valueOf(player)).intValue();
    }

    public InitConfig getInit()
    {
        return init;
    }

    public SplitType getSplit()
    {
        final SplitType type;
        if (players == 1)
        {
            type = SplitType.NONE;
        }
        else if (players == 2)
        {
            type = SplitType.TWO_HORIZONTAL;
        }
        else
        {
            type = SplitType.FOUR;
        }
        return type;
    }
}
