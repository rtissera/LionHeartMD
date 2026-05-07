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
 *
 * Stripped variant for C/SDL2 port reference: fog-of-war + map circuit code paths
 * removed (corresponding subpackages dropped from engine — Lionheart does not use
 * RTS-style fog or circuit auto-generation).
 */
package com.b3dgs.lionengine.helper;

import java.io.IOException;
import java.util.function.Consumer;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.b3dgs.lionengine.Media;
import com.b3dgs.lionengine.Medias;
import com.b3dgs.lionengine.game.feature.Camera;
import com.b3dgs.lionengine.game.feature.Factory;
import com.b3dgs.lionengine.game.feature.Handler;
import com.b3dgs.lionengine.game.feature.HandlerPersister;
import com.b3dgs.lionengine.game.feature.Services;
import com.b3dgs.lionengine.game.feature.tile.TileGroupsConfig;
import com.b3dgs.lionengine.game.feature.tile.map.MapTileAppenderModel;
import com.b3dgs.lionengine.game.feature.tile.map.MapTileGame;
import com.b3dgs.lionengine.game.feature.tile.map.MapTileGroup;
import com.b3dgs.lionengine.game.feature.tile.map.MapTileGroupModel;
import com.b3dgs.lionengine.game.feature.tile.map.TileSheetsConfig;
import com.b3dgs.lionengine.game.feature.tile.map.collision.CollisionFormulaConfig;
import com.b3dgs.lionengine.game.feature.tile.map.collision.CollisionGroupConfig;
import com.b3dgs.lionengine.game.feature.tile.map.collision.MapTileCollision;
import com.b3dgs.lionengine.game.feature.tile.map.collision.MapTileCollisionModel;
import com.b3dgs.lionengine.game.feature.tile.map.persister.MapTilePersister;
import com.b3dgs.lionengine.game.feature.tile.map.persister.MapTilePersisterListener;
import com.b3dgs.lionengine.game.feature.tile.map.persister.MapTilePersisterModel;
import com.b3dgs.lionengine.game.feature.tile.map.raster.MapTileRastered;
import com.b3dgs.lionengine.game.feature.tile.map.raster.MapTileRasteredModel;
import com.b3dgs.lionengine.game.feature.tile.map.transition.MapTileTransition;
import com.b3dgs.lionengine.game.feature.tile.map.transition.MapTileTransitionModel;
import com.b3dgs.lionengine.game.feature.tile.map.transition.TransitionsConfig;
import com.b3dgs.lionengine.game.feature.tile.map.viewer.MapTileViewer;
import com.b3dgs.lionengine.game.feature.tile.map.viewer.MapTileViewerModel;
import com.b3dgs.lionengine.io.FileWriting;

/**
 * Map tile helper implementation.
 */
public class MapTileHelper extends MapTileGame
{
    /** Logger. */
    private static final Logger LOGGER = LoggerFactory.getLogger(MapTileHelper.class);

    /**
     * Import and save the level.
     *
     * @param levelrip The level rip.
     * @param out The output media.
     */
    public static void importAndSave(Media levelrip, Media out)
    {
        importAndSave(levelrip, out, new MapTilePersisterModel());
    }

    /**
     * Import and save the level.
     *
     * @param levelrip The level rip.
     * @param out The output media.
     * @param mapPersister The persister reference.
     */
    public static void importAndSave(Media levelrip, Media out, MapTilePersister mapPersister)
    {
        importAndSave(levelrip, Medias.create(levelrip.getParentPath(), TileSheetsConfig.FILENAME), out, mapPersister);
    }

    /**
     * Import and save the level.
     *
     * @param levelrip The level rip.
     * @param sheetsConfig The file that define the sheets configuration.
     * @param out The output media.
     * @param mapPersister The persister reference.
     */
    public static void importAndSave(Media levelrip, Media sheetsConfig, Media out, MapTilePersister mapPersister)
    {
        final Services services = new Services();
        final MapTileGame map = services.create(MapTileGame.class);
        map.create(levelrip, sheetsConfig);
        map.addFeature(mapPersister);

        services.add(new Factory(services));
        services.add(new Handler(services));
        final HandlerPersister persister = new HandlerPersister(services);

        try (FileWriting output = new FileWriting(out))
        {
            mapPersister.save(output);
            persister.save(output);
        }
        catch (final IOException exception)
        {
            LOGGER.error("Error on saving map !", exception);
        }
    }

    /**
     * Load configuration from file.
     *
     * @param media The media parent.
     * @param loader The loader reference.
     * @param file The file to load.
     */
    private static void load(Media media, Consumer<Media> loader, String file)
    {
        final Media config = Medias.create(media.getParentPath(), file);
        if (config.exists())
        {
            loader.accept(config);
        }
    }

    private final MapTileGroup mapGroup;
    private final MapTileCollision mapCollision;
    private final MapTileTransition mapTransition;
    private final MapTileRastered mapRaster;
    private final MapTileViewer mapViewer;

    /**
     * Create helper.
     *
     * @param services The services reference.
     */
    public MapTileHelper(Services services)
    {
        super();

        mapGroup = addFeature(new MapTileGroupModel());
        mapCollision = addFeature(new MapTileCollisionModel());
        mapTransition = addFeature(new MapTileTransitionModel());
        mapRaster = addFeature(new MapTileRasteredModel());
        mapViewer = addFeature(new MapTileViewerModel(services));

        addFeature(new MapTileAppenderModel());
        addFeature(new MapTilePersisterModel()).addListener(new MapTilePersisterListener()
        {
            @Override
            public void notifyMapLoadStart()
            {
                loadBefore(getMedia());
            }

            @Override
            public void notifyMapLoaded()
            {
                loadAfter(getMedia());
                services.get(Camera.class).setLimits(mapSurface);
            }
        });
    }

    /**
     * Load features.
     *
     * @param media The parent media.
     */
    public void loadBefore(Media media)
    {
        if (media != null)
        {
            load(media, mapGroup::loadGroups, TileGroupsConfig.FILENAME);
        }
    }

    /**
     * Load features.
     *
     * @param media The parent media.
     */
    public void loadAfter(Media media)
    {
        if (media != null)
        {
            final String parent = media.getParentPath();

            final Media configFormulas = Medias.create(parent, CollisionFormulaConfig.FILENAME);
            if (configFormulas.exists())
            {
                mapCollision.loadCollisions(configFormulas, Medias.create(parent, CollisionGroupConfig.FILENAME));
            }
            load(media, mapTransition::loadTransitions, TransitionsConfig.FILENAME);

            if (mapRaster.loadSheets())
            {
                mapViewer.clear();
                mapViewer.addRenderer(mapRaster);
            }
        }
    }

    @Override
    public void create(int tileWidth, int tileHeight, int widthInTile, int heightInTile)
    {
        super.create(tileWidth, tileHeight, widthInTile, heightInTile);

        loadBefore(getMedia());
        loadAfter(getMedia());
    }

    @Override
    public void create(Media levelrip, int tileWidth, int tileHeight, int horizontalTiles)
    {
        super.create(levelrip, tileWidth, tileHeight, horizontalTiles);

        loadBefore(getMedia());
        loadAfter(levelrip);
    }

    @Override
    public void create(Media levelrip)
    {
        super.create(levelrip);

        loadBefore(getMedia());
        loadAfter(getMedia());
    }
}
