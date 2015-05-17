#!/bin/bash
copyLocation=../../CloudsData/visualsystems
cloudsdata=../../CloudsData
if [ ! -d $cloudsdata ]; then
mkdir $cloudsdata
mkdir $copyLocation
 fi
cp -r ../CloudsData/about ../../CloudsData/
cp -r ../CloudsData/backgrounds ../../CloudsData/
cp -r ../CloudsData/clipxml ../../CloudsData/
cp -r ../CloudsData/colors ../../CloudsData/
cp ../CloudsData/copyscript.sh ../../CloudsData/copyscript.sh
cp ../CloudsData/credits.xml ../../CloudsData/credits.xml
cp ../CloudsData/credits_backup.xml ../../CloudsData/credits_backup.xml
cp -r ../CloudsData/fcpxml ../../CloudsData/
cp -r ../CloudsData/font ../../CloudsData/
cp -r ../CloudsData/gephi ../../CloudsData/
cp -r ../CloudsData/graph ../../CloudsData/
cp -r ../CloudsData/HUD ../../CloudsData/
cp ../CloudsData/HUD.xml ../../CloudsData/HUD.xml
cp ../CloudsData/HUD_SD.xml ../../CloudsData/HUD_SD.xml
cp -r ../CloudsData/images ../../CloudsData/
cp -r ../CloudsData/input ../../CloudsData/
cp ../CloudsData/Kinect.xml ../../CloudsData/Kinect.xml
cp ../CloudsData/Kinect_EYEO.xml ../../CloudsData/Kinect_EYEO.xml
cp -r ../CloudsData/language ../../CloudsData/
cp ../CloudsData/language.txt ../../CloudsData/language.txt
cp -r ../CloudsData/links ../../CloudsData/
cp -r ../CloudsData/logs ../../CloudsData/
cp ../CloudsData/OculusRift.xml ../../CloudsData/OculusRift.xml
cp -r ../CloudsData/pajek ../../CloudsData/
cp -r ../CloudsData/runs ../../CloudsData/
cp -r ../CloudsData/secondaryDisplay ../../CloudsData/
cp -r ../CloudsData/shaders ../../CloudsData/
cp -r ../CloudsData/sound ../../CloudsData/
cp -r ../CloudsData/stats ../../CloudsData/
cp -r ../CloudsData/storyEngineParameters ../../CloudsData/
cp -r ../CloudsData/transitions ../../CloudsData/
cp ../CloudsData/TRANSLATED_question_topics.txt ../../CloudsData/TRANSLATED_question_topics.txt
cp -r ../CloudsData/vhx ../../CloudsData/
cp ../CloudsData/visual_quality_config.txt ../../CloudsData/visual_quality_config.txt
cp -r ../CloudsData/VO ../../CloudsData/
cp -r ../CloudsDataIgnored/sound ../../CloudsData/
mkdir ../../CloudsData/visualsystems/2DVideo
mkdir ../../CloudsData/visualsystems/2DVideo/Presets
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Alice_TextArc $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Cascade $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/D3_Dial $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/D3_Radial $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/D3_Timeline $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/D3_stock $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Exoplanets $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/FaceDebug_JapaneseCouple $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/FaceDebug_Teens $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/FaceShift_noisy $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/FaceSubstitution $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Flow1_gesture $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Flow2_Blue $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Flow3_Closeup $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/GitHubMap $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Gource $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Kittenstep $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/LiveCode3_DesignByNumbers $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Livecode2_networkgame $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Logo1 $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Logo2 $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/OpenPaths $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/PeopleStaring $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Reas_Process13 $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Reas_network1 $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/TweakMode $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/USMap $copyLocation/2DVideo/Presets/
cp -r ../CloudsData/visualsystems/2DVideo/Presets/Zipcode $copyLocation/2DVideo/Presets/
mkdir ../../CloudsData/visualsystems/3DModelLoader
mkdir ../../CloudsData/visualsystems/3DModelLoader/Presets
cp -r ../CloudsData/visualsystems/3DModelLoader/3DModelShaders $copyLocation/3DModelLoader/3DModelShaders
cp -r ../CloudsData/visualsystems/3DModelLoader/arrow.mtl $copyLocation/3DModelLoader/arrow.mtl
cp -r ../CloudsData/visualsystems/3DModelLoader/arrow.obj $copyLocation/3DModelLoader/arrow.obj
cp -r ../CloudsData/visualsystems/3DModelLoader/cameraPaths $copyLocation/3DModelLoader/cameraPaths
cp -r ../CloudsData/visualsystems/3DModelLoader/cameraVertices.txt $copyLocation/3DModelLoader/cameraVertices.txt
cp -r ../CloudsData/visualsystems/3DModelLoader/GUI $copyLocation/3DModelLoader/GUI
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/BigData $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Clock2 $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Kelp_Xray $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Megasphere_purple $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Nest_CoolTransparent $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Toxi_AllinOne_Normals $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Toxi_Ellipsoid2 $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Toxi_Megasphere $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Toxi_Sphere1 $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Transparency_Grenade $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Virus1 $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Vroi $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/voronoi $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/FallingCubes $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsData/visualsystems/3DModelLoader/Presets/Vortex $copyLocation/3DModelLoader/Presets/
cp -r ../CloudsDataIgnored/visualsystems/3DModelLoader/models_binary $copyLocation/3DModelLoader/models_binary
mkdir ../../CloudsData/visualsystems/_ClusterMap
mkdir ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/images $copyLocation/_ClusterMap/images
cp -r ../CloudsData/visualsystems/_ClusterMap/shaders $copyLocation/_ClusterMap/shaders
mkdir ../../CloudsData/visualsystems/_Intro
mkdir ../../CloudsData/visualsystems/_Intro/Presets
cp -r ../CloudsData/visualsystems/_Intro/images $copyLocation/_Intro/images
cp -r ../CloudsData/visualsystems/_Intro/OBJ $copyLocation/_Intro/OBJ
cp -r ../CloudsData/visualsystems/_Intro/shaders $copyLocation/_Intro/shaders
cp -r ../CloudsData/visualsystems/_Intro/snapshots $copyLocation/_Intro/snapshots
mkdir ../../CloudsData/visualsystems/Astrolabe
mkdir ../../CloudsData/visualsystems/Astrolabe/Presets
cp -r ../CloudsData/visualsystems/Astrolabe/models $copyLocation/Astrolabe/models
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/Clock2 $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/BigBlueDial $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/BigDial_Linear_ZoomIn $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/BigDial_PanZoomout $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/BigDial_ZoomIn $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/Clock2_ZoomIn $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/Clock3_ZoomOut $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/DreamEggs1 $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/GravityClock_a $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/notTick $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/Presets/variedSpeed $copyLocation/Astrolabe/Presets/
cp -r ../CloudsData/visualsystems/Astrolabe/shaders $copyLocation/Astrolabe/shaders
cp -r ../CloudsDataIgnored/visualsystems/Astrolabe/models $copyLocation/Astrolabe/models
mkdir ../../CloudsData/visualsystems/Automata
mkdir ../../CloudsData/visualsystems/Automata/Presets
cp -r ../CloudsData/visualsystems/Automata/Presets/ConwayClassic_ZoomUp $copyLocation/Automata/Presets/
cp -r ../CloudsData/visualsystems/Automata/Presets/SlimeMold_zoomOut $copyLocation/Automata/Presets/
cp -r ../CloudsData/visualsystems/Automata/Presets/jm_Conway_2DZoom $copyLocation/Automata/Presets/
cp -r ../CloudsData/visualsystems/Automata/seedImages $copyLocation/Automata/seedImages
cp -r ../CloudsData/visualsystems/Automata/shaders $copyLocation/Automata/shaders
mkdir ../../CloudsData/visualsystems/BallDroppings
mkdir ../../CloudsData/visualsystems/BallDroppings/Presets
cp -r ../CloudsData/visualsystems/BallDroppings/Presets/gal1 $copyLocation/BallDroppings/Presets/
cp -r ../CloudsData/visualsystems/BallDroppings/Presets/jm $copyLocation/BallDroppings/Presets/
cp -r ../CloudsData/visualsystems/BallDroppings/Presets/gal1 $copyLocation/BallDroppings/Presets/
cp -r ../CloudsData/visualsystems/BallDroppings/shaders $copyLocation/BallDroppings/shaders
cp -r ../CloudsData/visualsystems/BallDroppings/sine.wav $copyLocation/BallDroppings/sine.wav
mkdir ../../CloudsData/visualsystems/Balloons
mkdir ../../CloudsData/visualsystems/Balloons/Presets
cp -r ../CloudsData/visualsystems/Balloons/images $copyLocation/Balloons/images
cp -r ../CloudsData/visualsystems/Balloons/models $copyLocation/Balloons/models
cp -r ../CloudsData/visualsystems/Balloons/shaders $copyLocation/Balloons/shaders
cp -r ../CloudsData/visualsystems/Balloons/sphericalMaps $copyLocation/Balloons/sphericalMaps
mkdir ../../CloudsData/visualsystems/CandyMountains
mkdir ../../CloudsData/visualsystems/CandyMountains/Presets
cp -r ../CloudsData/visualsystems/CandyMountains/dataMinMax.xml $copyLocation/CandyMountains/dataMinMax.xml
cp -r ../CloudsData/visualsystems/CandyMountains/faucet.ttf $copyLocation/CandyMountains/faucet.ttf
cp -r ../CloudsData/visualsystems/CandyMountains/shaders $copyLocation/CandyMountains/shaders
mkdir ../../CloudsData/visualsystems/Chromogram
mkdir ../../CloudsData/visualsystems/Chromogram/Presets
mkdir ../../CloudsData/visualsystems/CirclePacking
mkdir ../../CloudsData/visualsystems/CirclePacking/Presets
cp -r ../CloudsData/visualsystems/CirclePacking/Courier.ttf $copyLocation/CirclePacking/Courier.ttf
cp -r ../CloudsData/visualsystems/CirclePacking/frabk.ttf $copyLocation/CirclePacking/frabk.ttf
cp -r ../CloudsData/visualsystems/CirclePacking/NewMedia.ttf $copyLocation/CirclePacking/NewMedia.ttf
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/Blank $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/Circles_Social $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/Circles_Social2 $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/Circles_Social3 $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/Circles_VariegatedBiological $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/Circles_Watery $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/Presets/NASDAQ $copyLocation/CirclePacking/Presets/
cp -r ../CloudsData/visualsystems/CirclePacking/verdana14.ttf $copyLocation/CirclePacking/verdana14.ttf
mkdir ../../CloudsData/visualsystems/CirclePackingSystem
mkdir ../../CloudsData/visualsystems/CirclePackingSystem/Presets
cp -r ../CloudsData/visualsystems/CirclePackingSystem/Presets/Blank $copyLocation/CirclePackingSystem/Presets/
cp -r ../CloudsData/visualsystems/CirclePackingSystem/Presets/NASDAQ $copyLocation/CirclePackingSystem/Presets/
mkdir ../../CloudsData/visualsystems/Circuit
mkdir ../../CloudsData/visualsystems/Circuit/Presets
cp -r ../CloudsData/visualsystems/Circuit/Presets/JG_AggregiousSweep $copyLocation/Circuit/Presets/
cp -r ../CloudsData/visualsystems/Circuit/Presets/JM_LongSweep $copyLocation/Circuit/Presets/
cp -r ../CloudsData/visualsystems/Circuit/Presets/JM_ZoomOut $copyLocation/Circuit/Presets/
cp -r ../CloudsData/visualsystems/Circuit/Presets/JM_ZoomOutIn $copyLocation/Circuit/Presets/
cp -r ../CloudsData/visualsystems/Circuit/shaders $copyLocation/Circuit/shaders
cp -r ../CloudsDataIgnored/visualsystems/Circuit/images $copyLocation/Circuit/images
mkdir ../../CloudsData/visualsystems/Cities
mkdir ../../CloudsData/visualsystems/Cities/Presets
cp -r ../CloudsData/visualsystems/Cities/images $copyLocation/Cities/images
cp -r ../CloudsData/visualsystems/Cities/models $copyLocation/Cities/models
cp -r ../CloudsData/visualsystems/Cities/Presets/Archiving $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/Archiving6 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/Sprawl_aerial1 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/ThinSkyscrapers1 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/gx_FIN_01 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/gx_FIN_02 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/gx_work_02a-1 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/realistic2 $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/gx_work_02a $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/gx_work_02a $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/Presets/CLOUDS $copyLocation/Cities/Presets/
cp -r ../CloudsData/visualsystems/Cities/shaders $copyLocation/Cities/shaders
mkdir ../../CloudsData/visualsystems/Code
mkdir ../../CloudsData/visualsystems/Code/Presets
cp -r ../CloudsData/visualsystems/Code/code_test.txt $copyLocation/Code/code_test.txt
cp -r ../CloudsData/visualsystems/Code/Presets/Basic2 $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/Code_Console_default $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/Code_default $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/Cryptic $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/JG_BlueCascade $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/JG_Spreadsheet $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/JG_TenPrintCascade $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/JM_BlueCascade2 $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/TenPrint $copyLocation/Code/Presets/
cp -r ../CloudsData/visualsystems/Code/Presets/basic1 $copyLocation/Code/Presets/
mkdir ../../CloudsData/visualsystems/Colony
mkdir ../../CloudsData/visualsystems/Colony/Presets
cp -r ../CloudsData/visualsystems/Colony/Presets/Crazy $copyLocation/Colony/Presets/
cp -r ../CloudsData/visualsystems/Colony/Presets/Phytoplankton $copyLocation/Colony/Presets/
cp -r ../CloudsData/visualsystems/Colony/Presets/QuantumDecoherence_jm $copyLocation/Colony/Presets/
cp -r ../CloudsData/visualsystems/Colony/Presets/RapidCulture $copyLocation/Colony/Presets/
cp -r ../CloudsData/visualsystems/Colony/Presets/SyntheticBlack $copyLocation/Colony/Presets/
cp -r ../CloudsData/visualsystems/Colony/Presets/SyntheticWhite $copyLocation/Colony/Presets/
cp -r ../CloudsData/visualsystems/Colony/shaders $copyLocation/Colony/shaders
cp -r ../CloudsData/visualsystems/Colony/sprites $copyLocation/Colony/sprites
cp -r ../CloudsData/visualsystems/Colony/textures $copyLocation/Colony/textures
mkdir ../../CloudsData/visualsystems/Connectors
mkdir ../../CloudsData/visualsystems/Connectors/Presets
cp -r ../CloudsData/visualsystems/Connectors/Presets/Fast_connections $copyLocation/Connectors/Presets/
cp -r ../CloudsData/visualsystems/Connectors/Presets/Rotating_Box $copyLocation/Connectors/Presets/
cp -r ../CloudsData/visualsystems/Connectors/Presets/Rotating_Box_SND $copyLocation/Connectors/Presets/
cp -r ../CloudsData/visualsystems/Connectors/Presets/Rotating_Box_slow $copyLocation/Connectors/Presets/
cp -r ../CloudsData/visualsystems/Connectors/Presets/g_work_02a $copyLocation/Connectors/Presets/
cp -r ../CloudsData/visualsystems/Connectors/Presets/g_work_02a $copyLocation/Connectors/Presets/
mkdir ../../CloudsData/visualsystems/Cosmic
mkdir ../../CloudsData/visualsystems/Cosmic/Presets
cp -r ../CloudsData/visualsystems/Cosmic/audio $copyLocation/Cosmic/audio
cp -r ../CloudsData/visualsystems/Cosmic/colors $copyLocation/Cosmic/colors
cp -r ../CloudsData/visualsystems/Cosmic/images $copyLocation/Cosmic/images
cp -r ../CloudsData/visualsystems/Cosmic/noise.png $copyLocation/Cosmic/noise.png
cp -r ../CloudsData/visualsystems/Cosmic/Presets/Attractors $copyLocation/Cosmic/Presets/
cp -r ../CloudsData/visualsystems/Cosmic/Presets/Blackhole $copyLocation/Cosmic/Presets/
cp -r ../CloudsData/visualsystems/Cosmic/Presets/EG_CosmicCornea $copyLocation/Cosmic/Presets/
cp -r ../CloudsData/visualsystems/Cosmic/Presets/nBody $copyLocation/Cosmic/Presets/
cp -r ../CloudsData/visualsystems/Cosmic/Presets/nBody_green $copyLocation/Cosmic/Presets/
cp -r ../CloudsData/visualsystems/Cosmic/shaders $copyLocation/Cosmic/shaders
mkdir ../../CloudsData/visualsystems/CubeCraft
mkdir ../../CloudsData/visualsystems/CubeCraft/Presets
cp -r ../CloudsData/visualsystems/CubeCraft/models $copyLocation/CubeCraft/models
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/g_work_02a $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/MC_desert2 $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/MC_desert3 $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/MC_infinite $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/MC_shadows $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/MC_shadows_1 $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/Tetris $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/g_FIN_02 $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/g_work_02a $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/j_05_blue $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/j_FIN_01 $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/j_work_05a_bluegreen $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/Presets/lewitt $copyLocation/CubeCraft/Presets/
cp -r ../CloudsData/visualsystems/CubeCraft/shaders $copyLocation/CubeCraft/shaders
mkdir ../../CloudsData/visualsystems/DataCascade
mkdir ../../CloudsData/visualsystems/DataCascade/Presets
cp -r ../CloudsData/visualsystems/DataCascade/Presets/DataCascade $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/DataCascadeClose $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/DataCascade_JG_Intro_Sweept $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/DoubleTunnel $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/DoubleTunnel2 $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/RGBDCascade $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/UpwardStream $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/gx_work_02b $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/gx_work_02a $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/gx_work_02a $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/gx_work_03a $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/Presets/gx_work_04a $copyLocation/DataCascade/Presets/
cp -r ../CloudsData/visualsystems/DataCascade/shaders $copyLocation/DataCascade/shaders
mkdir ../../CloudsData/visualsystems/DrawnLine
mkdir ../../CloudsData/visualsystems/DrawnLine/Presets
cp -r ../CloudsData/visualsystems/DrawnLine/other $copyLocation/DrawnLine/other
cp -r ../CloudsData/visualsystems/DrawnLine/Presets/NarrativeLine $copyLocation/DrawnLine/Presets/
cp -r ../CloudsData/visualsystems/DrawnLine/shaders $copyLocation/DrawnLine/shaders
mkdir ../../CloudsData/visualsystems/Empty
mkdir ../../CloudsData/visualsystems/Empty/Presets
cp -r ../CloudsData/visualsystems/Empty/snapshots $copyLocation/Empty/snapshots
mkdir ../../CloudsData/visualsystems/ExampleBox2D
mkdir ../../CloudsData/visualsystems/ExampleBox2D/Presets
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM1 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM2 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM_GravityCircles $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM_Squares $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM_circleSquares1 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM_gravity1 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM_gravity2 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/JM1 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/gal2 $copyLocation/ExampleBox2D/Presets/
cp -r ../CloudsData/visualsystems/ExampleBox2D/Presets/gal2 $copyLocation/ExampleBox2D/Presets/
mkdir ../../CloudsData/visualsystems/ExampleMPMFluid
mkdir ../../CloudsData/visualsystems/ExampleMPMFluid/Presets
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/gal1 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/JM1 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/EG_BreathingLiquid $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/EG_LiquidBreathing2 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/FerroFluid $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/Flow $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/Flow_superfast $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/Flow_superfast2 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/JM1 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/Taffy $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/gal1 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/gal2 $copyLocation/ExampleMPMFluid/Presets/
cp -r ../CloudsData/visualsystems/ExampleMPMFluid/Presets/gal2 $copyLocation/ExampleMPMFluid/Presets/
mkdir ../../CloudsData/visualsystems/ExampleOpenSteer
mkdir ../../CloudsData/visualsystems/ExampleOpenSteer/Presets
cp -r ../CloudsData/visualsystems/ExampleOpenSteer/Presets/Fish_purple $copyLocation/ExampleOpenSteer/Presets/
cp -r ../CloudsData/visualsystems/ExampleOpenSteer/Presets/Pursuit_gray $copyLocation/ExampleOpenSteer/Presets/
cp -r ../CloudsData/visualsystems/ExampleOpenSteer/Presets/Streamers_purple $copyLocation/ExampleOpenSteer/Presets/
mkdir ../../CloudsData/visualsystems/ExampleVectorMath
mkdir ../../CloudsData/visualsystems/ExampleVectorMath/Presets
cp -r ../CloudsData/visualsystems/ExampleVectorMath/Presets/DrawYourOwnRainbow $copyLocation/ExampleVectorMath/Presets/
cp -r ../CloudsData/visualsystems/ExampleVectorMath/Presets/DrawYourOwnRainbowOC $copyLocation/ExampleVectorMath/Presets/
cp -r ../CloudsData/visualsystems/ExampleVectorMath/Presets/WhiteLine $copyLocation/ExampleVectorMath/Presets/
mkdir ../../CloudsData/visualsystems/Fireworks
mkdir ../../CloudsData/visualsystems/Fireworks/Presets
cp -r ../CloudsData/visualsystems/Fireworks/animationTargets $copyLocation/Fireworks/animationTargets
cp -r ../CloudsData/visualsystems/Fireworks/images $copyLocation/Fireworks/images
cp -r ../CloudsData/visualsystems/Fireworks/Presets/g_work_02a $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/g_work_02a $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/Attack $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/Attack_a $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/Pigment_b $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/Pigment_e $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/Skydiving_1 $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/SlowMotion $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/Traditional_a $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/WildBlue $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/Presets/underwater $copyLocation/Fireworks/Presets/
cp -r ../CloudsData/visualsystems/Fireworks/shaders $copyLocation/Fireworks/shaders
mkdir ../../CloudsData/visualsystems/Flocking
mkdir ../../CloudsData/visualsystems/Flocking/Presets
cp -r ../CloudsData/visualsystems/Flocking/Presets/JM1 $copyLocation/Flocking/Presets/
cp -r ../CloudsData/visualsystems/Flocking/Presets/JM2 $copyLocation/Flocking/Presets/
cp -r ../CloudsData/visualsystems/Flocking/Presets/JM1 $copyLocation/Flocking/Presets/
cp -r ../CloudsData/visualsystems/Flocking/shaders $copyLocation/Flocking/shaders
mkdir ../../CloudsData/visualsystems/Flying
mkdir ../../CloudsData/visualsystems/Flying/Presets
cp -r ../CloudsData/visualsystems/Flying/images $copyLocation/Flying/images
cp -r ../CloudsData/visualsystems/Flying/Presets/DenseForest_1 $copyLocation/Flying/Presets/
cp -r ../CloudsData/visualsystems/Flying/Presets/DenseForest_SND $copyLocation/Flying/Presets/
cp -r ../CloudsData/visualsystems/Flying/Presets/lush $copyLocation/Flying/Presets/
cp -r ../CloudsData/visualsystems/Flying/rules $copyLocation/Flying/rules
cp -r ../CloudsData/visualsystems/Flying/shaders $copyLocation/Flying/shaders
mkdir ../../CloudsData/visualsystems/ForkingPaths
mkdir ../../CloudsData/visualsystems/ForkingPaths/Presets
mkdir ../../CloudsData/visualsystems/GesturePaint
mkdir ../../CloudsData/visualsystems/GesturePaint/Presets
cp -r ../CloudsData/visualsystems/GesturePaint/images $copyLocation/GesturePaint/images
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/BlackOnWhite $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/Richter1 $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/Richter2 $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/RothkoSunset $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/Turrell $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/WhiteOnBlack $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/Presets/CLOUDS $copyLocation/GesturePaint/Presets/
cp -r ../CloudsData/visualsystems/GesturePaint/shaders $copyLocation/GesturePaint/shaders
mkdir ../../CloudsData/visualsystems/GPUParticles
mkdir ../../CloudsData/visualsystems/GPUParticles/Presets
cp -r ../CloudsData/visualsystems/GPUParticles/glow.png $copyLocation/GPUParticles/glow.png
cp -r ../CloudsData/visualsystems/GPUParticles/glow1.png $copyLocation/GPUParticles/glow1.png
cp -r ../CloudsData/visualsystems/GPUParticles/glow2.png $copyLocation/GPUParticles/glow2.png
cp -r ../CloudsData/visualsystems/GPUParticles/glow3.png $copyLocation/GPUParticles/glow3.png
cp -r ../CloudsData/visualsystems/GPUParticles/glow5.png $copyLocation/GPUParticles/glow5.png
cp -r ../CloudsData/visualsystems/GPUParticles/nipple.png $copyLocation/GPUParticles/nipple.png
cp -r ../CloudsData/visualsystems/GPUParticles/nipple3.png $copyLocation/GPUParticles/nipple3.png
cp -r ../CloudsData/visualsystems/GPUParticles/noisy.png $copyLocation/GPUParticles/noisy.png
cp -r ../CloudsData/visualsystems/GPUParticles/shaders $copyLocation/GPUParticles/shaders
cp -r ../CloudsData/visualsystems/GPUParticles/spark.png $copyLocation/GPUParticles/spark.png
cp -r ../CloudsData/visualsystems/GPUParticles/sprites $copyLocation/GPUParticles/sprites
mkdir ../../CloudsData/visualsystems/Histogram
mkdir ../../CloudsData/visualsystems/Histogram/Presets
cp -r ../CloudsData/visualsystems/Histogram/Presets/DataHistogram2 $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/DataHistogram3 $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/DataHistogram4 $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/Waveform1 $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/Waveform1_Happen $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/Waveform2 $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/Waveform3 $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/Waveform3_GTNonStop $copyLocation/Histogram/Presets/
cp -r ../CloudsData/visualsystems/Histogram/Presets/land2 $copyLocation/Histogram/Presets/
cp -r ../CloudsDataIgnored/visualsystems/Histogram/sounds $copyLocation/Histogram/sounds
mkdir ../../CloudsData/visualsystems/LaplacianTunnel
mkdir ../../CloudsData/visualsystems/LaplacianTunnel/Presets
cp -r ../CloudsData/visualsystems/LaplacianTunnel/Presets/LaplacianBlack $copyLocation/LaplacianTunnel/Presets/
cp -r ../CloudsData/visualsystems/LaplacianTunnel/Presets/LaplacianGray $copyLocation/LaplacianTunnel/Presets/
cp -r ../CloudsData/visualsystems/LaplacianTunnel/Presets/LaplacianPointsOC $copyLocation/LaplacianTunnel/Presets/
cp -r ../CloudsData/visualsystems/LaplacianTunnel/Presets/LaplacianWhite $copyLocation/LaplacianTunnel/Presets/
cp -r ../CloudsData/visualsystems/LaplacianTunnel/Presets/White_5Reps_Slow $copyLocation/LaplacianTunnel/Presets/
cp -r ../CloudsData/visualsystems/LaplacianTunnel/shaders $copyLocation/LaplacianTunnel/shaders
cp -r ../CloudsDataIgnored/visualsystems/LaplacianTunnel/Meshes $copyLocation/LaplacianTunnel/Meshes
mkdir ../../CloudsData/visualsystems/LIA
mkdir ../../CloudsData/visualsystems/LIA/Presets
cp -r ../CloudsData/visualsystems/LIA/Presets/JM_White $copyLocation/LIA/Presets/
cp -r ../CloudsData/visualsystems/LIA/Presets/LIA_07 $copyLocation/LIA/Presets/
cp -r ../CloudsData/visualsystems/LIA/Presets/LIA_10 $copyLocation/LIA/Presets/
cp -r ../CloudsDataIgnored/visualsystems/LIA/images $copyLocation/LIA/images
mkdir ../../CloudsData/visualsystems/LinePoem
mkdir ../../CloudsData/visualsystems/LinePoem/Presets
cp -r ../CloudsData/visualsystems/LinePoem/allDrawings.dat $copyLocation/LinePoem/allDrawings.dat
mkdir ../../CloudsData/visualsystems/LSystem
mkdir ../../CloudsData/visualsystems/LSystem/Presets
cp -r ../CloudsData/visualsystems/LSystem/images $copyLocation/LSystem/images
cp -r ../CloudsData/visualsystems/LSystem/Presets/gx_work_02a $copyLocation/LSystem/Presets/
cp -r ../CloudsData/visualsystems/LSystem/Presets/gx_work_01a $copyLocation/LSystem/Presets/
cp -r ../CloudsData/visualsystems/LSystem/Presets/gx_work_02a $copyLocation/LSystem/Presets/
cp -r ../CloudsData/visualsystems/LSystem/Presets/gx_work_01a $copyLocation/LSystem/Presets/
cp -r ../CloudsData/visualsystems/LSystem/Presets/gx_work_01a $copyLocation/LSystem/Presets/
cp -r ../CloudsData/visualsystems/LSystem/Presets/gx_work_01a $copyLocation/LSystem/Presets/
cp -r ../CloudsData/visualsystems/LSystem/shaders $copyLocation/LSystem/shaders
mkdir ../../CloudsData/visualsystems/Mandala
mkdir ../../CloudsData/visualsystems/Mandala/Presets
cp -r ../CloudsData/visualsystems/Mandala/maya_scripts $copyLocation/Mandala/maya_scripts
cp -r ../CloudsData/visualsystems/Mandala/models $copyLocation/Mandala/models
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_02b $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_02a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/BollywoodWedding_1 $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/DinoSkinPlanetOC $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/HeartMachine $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/JM_AlienFusionReactor $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/JM_AlienFusionReactor2 $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/JM_AlienFusionReactor2OC $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/JM_pulse1OC $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/PureForm $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/PureFormEvolving_OC $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/PureFormOC $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_01a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_02a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_01a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_03a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_04a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_01a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/Presets/gx_work_01a $copyLocation/Mandala/Presets/
cp -r ../CloudsData/visualsystems/Mandala/shaders $copyLocation/Mandala/shaders
mkdir ../../CloudsData/visualsystems/MarchingCubes
mkdir ../../CloudsData/visualsystems/MarchingCubes/Presets
cp -r ../CloudsData/visualsystems/MarchingCubes/images $copyLocation/MarchingCubes/images
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/M_TriBlobsSmooth $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/M_TriBlobsSmoothOC $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/MetaMarch_1 $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/MetaMarch_1OC $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/MetaMarch_2 $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/MetaMarch_2OC $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/MetaMarch_3 $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/MetaMarch_3OC $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/Presets/SineWire1 $copyLocation/MarchingCubes/Presets/
cp -r ../CloudsData/visualsystems/MarchingCubes/shaders $copyLocation/MarchingCubes/shaders
mkdir ../../CloudsData/visualsystems/MazeGenerator
mkdir ../../CloudsData/visualsystems/MazeGenerator/Presets
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/gal1 $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/gal1 $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/gal2 $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/Flyover3 $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/JM_flyover $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/JM_groundcam $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/flyover_near $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/gal2 $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/Presets/gal2OC $copyLocation/MazeGenerator/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator/shaders $copyLocation/MazeGenerator/shaders
mkdir ../../CloudsData/visualsystems/MazeGenerator2
mkdir ../../CloudsData/visualsystems/MazeGenerator2/Presets
cp -r ../CloudsData/visualsystems/MazeGenerator2/Presets/Generator1 $copyLocation/MazeGenerator2/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator2/Presets/Generator2_wide $copyLocation/MazeGenerator2/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator2/Presets/Generator3_wide $copyLocation/MazeGenerator2/Presets/
cp -r ../CloudsData/visualsystems/MazeGenerator2/Presets/Generator4_wide $copyLocation/MazeGenerator2/Presets/
mkdir ../../CloudsData/visualsystems/Memory
mkdir ../../CloudsData/visualsystems/Memory/Presets
cp -r ../CloudsData/visualsystems/Memory/images $copyLocation/Memory/images
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_02a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_01a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_02a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/DataOverload $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/DeFrag $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/Fire $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/Glitch $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/GlitchSort $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/ThinBlueRows $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/ThinBlueRows_2 $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/ThinBlueRows_wide $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/ThinTealRows $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/ThinTealRows2 $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/WideLEDCells2 $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_01a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_03b $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/randomWaves $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_03a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_04a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_01a $copyLocation/Memory/Presets/
cp -r ../CloudsData/visualsystems/Memory/Presets/gx_work_01a $copyLocation/Memory/Presets/
mkdir ../../CloudsData/visualsystems/Metaballs
mkdir ../../CloudsData/visualsystems/Metaballs/Presets
cp -r ../CloudsData/visualsystems/Metaballs/shaders $copyLocation/Metaballs/shaders
mkdir ../../CloudsData/visualsystems/Nbody
mkdir ../../CloudsData/visualsystems/Nbody/Presets
cp -r ../CloudsData/visualsystems/Nbody/shaders $copyLocation/Nbody/shaders
mkdir ../../CloudsData/visualsystems/Neurons
mkdir ../../CloudsData/visualsystems/Neurons/Presets
cp -r ../CloudsData/visualsystems/Neurons/Presets/Brain $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/Brain_wide $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/Brain_wide_OC $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/MezzyNeurons $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/MezzyNeuronsOC $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/MezzyNeurons_Q $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/VeryNervousSystem $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/VeryNervousSystem_OC $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/XmasDendrites $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/Presets/XmasDendrites_OC $copyLocation/Neurons/Presets/
cp -r ../CloudsData/visualsystems/Neurons/sets $copyLocation/Neurons/sets
cp -r ../CloudsData/visualsystems/Neurons/shaders $copyLocation/Neurons/shaders
mkdir ../../CloudsData/visualsystems/Ocean
mkdir ../../CloudsData/visualsystems/Ocean/Presets
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_FIN_02 $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/ChoppyBlueOcean $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/DayNight_SND $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/DunesAtNight $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/DunesAtNightOC $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/DuskOcean_JG $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/Exoplanet_dusk_walkOC $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/Glum $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/Mars_Noon_walk $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/Mars_morning_walk $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/NightOceanOC $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/NightOcean_SND $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/StormyOceanOC $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/WireframeNightOcean $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_FIN_01-1 $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_FIN_01_Epic $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_FIN_02_Monochrome $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_FIN_02_Red $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_epicLines_01OC $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_flatRadicalOC $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/Presets/g_gentleWaves $copyLocation/Ocean/Presets/
cp -r ../CloudsData/visualsystems/Ocean/shaders $copyLocation/Ocean/shaders
mkdir ../../CloudsData/visualsystems/OpenP53DIntro
mkdir ../../CloudsData/visualsystems/OpenP53DIntro/Presets
mkdir ../../CloudsData/visualsystems/OpenP5DrawingMachine10
mkdir ../../CloudsData/visualsystems/OpenP5DrawingMachine10/Presets
cp -r ../CloudsData/visualsystems/OpenP5DrawingMachine10/shaders $copyLocation/OpenP5DrawingMachine10/shaders
mkdir ../../CloudsData/visualsystems/OpenP5Fifteen
mkdir ../../CloudsData/visualsystems/OpenP5Fifteen/Presets
cp -r ../CloudsData/visualsystems/OpenP5Fifteen/shaders $copyLocation/OpenP5Fifteen/shaders
mkdir ../../CloudsData/visualsystems/OpenP5Hackpact
mkdir ../../CloudsData/visualsystems/OpenP5Hackpact/Presets
cp -r ../CloudsData/visualsystems/OpenP5Hackpact/Presets/JM_1_blue $copyLocation/OpenP5Hackpact/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Hackpact/Presets/JM_1_bluer $copyLocation/OpenP5Hackpact/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Hackpact/Presets/JM_1_bw $copyLocation/OpenP5Hackpact/Presets/
mkdir ../../CloudsData/visualsystems/OpenP5Machine
mkdir ../../CloudsData/visualsystems/OpenP5Machine/Presets
cp -r ../CloudsData/visualsystems/OpenP5Machine/Presets/JM_Machine1 $copyLocation/OpenP5Machine/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Machine/Presets/JM_Machine2 $copyLocation/OpenP5Machine/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Machine/Presets/JM_clockwork $copyLocation/OpenP5Machine/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Machine/Presets/JM_clockworkOC $copyLocation/OpenP5Machine/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Machine/Presets/JM_clockwork_SND $copyLocation/OpenP5Machine/Presets/
mkdir ../../CloudsData/visualsystems/OpenP5NoiseSphere
mkdir ../../CloudsData/visualsystems/OpenP5NoiseSphere/Presets
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_02a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_01a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_02a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_01a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/CoolEscalation $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/InYourFace $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/Nervous_Colors $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/Nervous_ColorsOC $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_03a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_04a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_02aSun_things1 $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_02aSun_things1OC $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_02aSun_things2 $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_02aSun_things2OC $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_03a_things2 $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_03a_things2OC $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_03a_things3 $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_03a_things3OC $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_01a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/jm_work_03a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/Presets/gx_work_01a $copyLocation/OpenP5NoiseSphere/Presets/
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/shaders $copyLocation/OpenP5NoiseSphere/shaders
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/snapshots $copyLocation/OpenP5NoiseSphere/snapshots
cp -r ../CloudsData/visualsystems/OpenP5NoiseSphere/sounds $copyLocation/OpenP5NoiseSphere/sounds
mkdir ../../CloudsData/visualsystems/OpenP5SeaOfLines
mkdir ../../CloudsData/visualsystems/OpenP5SeaOfLines/Presets
cp -r ../CloudsData/visualsystems/OpenP5SeaOfLines/Presets/BlackLines_1 $copyLocation/OpenP5SeaOfLines/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SeaOfLines/Presets/BlackLines_connect $copyLocation/OpenP5SeaOfLines/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SeaOfLines/Presets/FallingFireweb $copyLocation/OpenP5SeaOfLines/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SeaOfLines/Presets/FallingGreenWeb $copyLocation/OpenP5SeaOfLines/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SeaOfLines/Presets/FireWeb_mellow $copyLocation/OpenP5SeaOfLines/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SeaOfLines/Presets/WhiteLines_2 $copyLocation/OpenP5SeaOfLines/Presets/
mkdir ../../CloudsData/visualsystems/OpenP5SpaceJunk
mkdir ../../CloudsData/visualsystems/OpenP5SpaceJunk/Presets
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/geometry $copyLocation/OpenP5SpaceJunk/geometry
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_02a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_01a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_02a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_01a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_03a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/DynamicLighting $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/FallingCubes $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/FallingCubes_Chaos $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/JM_Sweep_Enormous $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/StarkCubes $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/architecture_lighting $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_01a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/jg_BlueSweep $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/jm_work_03a $copyLocation/OpenP5SpaceJunk/Presets/
cp -r ../CloudsData/visualsystems/OpenP5SpaceJunk/Presets/gx_work_01a $copyLocation/OpenP5SpaceJunk/Presets/
mkdir ../../CloudsData/visualsystems/OpenP5Spaghetti
mkdir ../../CloudsData/visualsystems/OpenP5Spaghetti/Presets
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/GUI $copyLocation/OpenP5Spaghetti/GUI
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/Expanding_galaxy $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/Fuzzy_galaxy $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/Fuzzy_galaxyOC $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/Galaxy_HotColdOC $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/Green_Lidar2 $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/Green_Lidar2OC $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/SmoothScribblesOC $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/VioletBlue_Nebula $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/purple_gold_pointcloud $copyLocation/OpenP5Spaghetti/Presets/
cp -r ../CloudsData/visualsystems/OpenP5Spaghetti/Presets/scribbles $copyLocation/OpenP5Spaghetti/Presets/
mkdir ../../CloudsData/visualsystems/OpenP5SpinningSolids
mkdir ../../CloudsData/visualsystems/OpenP5SpinningSolids/Presets
mkdir ../../CloudsData/visualsystems/OpenP5TextUniverse
mkdir ../../CloudsData/visualsystems/OpenP5TextUniverse/Presets
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/fonts $copyLocation/OpenP5TextUniverse/fonts
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Borges_random $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Borges_randomOC $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Bruce_DigitalHumanities $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Bruce_TextualTechnologies $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/FavoriteWords $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Kyle_Storytelling $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Lauren_LanguagesAsTools $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Lauren_SentimentAnalysis $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/Martin_AlltheWords $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/twitter $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/Presets/twitterOC $copyLocation/OpenP5TextUniverse/Presets/
cp -r ../CloudsData/visualsystems/OpenP5TextUniverse/textFiles $copyLocation/OpenP5TextUniverse/textFiles
mkdir ../../CloudsData/visualsystems/Orbit
mkdir ../../CloudsData/visualsystems/Orbit/Presets
cp -r ../CloudsData/visualsystems/Orbit/Presets/Lorenz_Dolly $copyLocation/Orbit/Presets/
cp -r ../CloudsData/visualsystems/Orbit/shaders $copyLocation/Orbit/shaders
mkdir ../../CloudsData/visualsystems/Oscillations
mkdir ../../CloudsData/visualsystems/Oscillations/Presets
cp -r ../CloudsData/visualsystems/Oscillations/Presets/gx_work_01a $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/gx_work_01a $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/gx_work_01a $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/Helix-Inverted $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/Progressive_wormhole_1 $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/Televisual $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/Waves $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/g_waveForm $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/gx_work_01a $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/Presets/laser_blast $copyLocation/Oscillations/Presets/
cp -r ../CloudsData/visualsystems/Oscillations/shaders $copyLocation/Oscillations/shaders
mkdir ../../CloudsData/visualsystems/Pages
mkdir ../../CloudsData/visualsystems/Pages/Presets
cp -r ../CloudsData/visualsystems/Pages/Presets/Drift $copyLocation/Pages/Presets/
cp -r ../CloudsData/visualsystems/Pages/Presets/DriftOC $copyLocation/Pages/Presets/
mkdir ../../CloudsData/visualsystems/PaintBrush
mkdir ../../CloudsData/visualsystems/PaintBrush/Presets
cp -r ../CloudsData/visualsystems/PaintBrush/colorMaps $copyLocation/PaintBrush/colorMaps
cp -r ../CloudsData/visualsystems/PaintBrush/Presets/Turrell $copyLocation/PaintBrush/Presets/
mkdir ../../CloudsData/visualsystems/Paradox
mkdir ../../CloudsData/visualsystems/Paradox/Presets
cp -r ../CloudsData/visualsystems/Paradox/Presets/GridZoom_2 $copyLocation/Paradox/Presets/
cp -r ../CloudsData/visualsystems/Paradox/Presets/Sphere1 $copyLocation/Paradox/Presets/
cp -r ../CloudsData/visualsystems/Paradox/Presets/TwistingBlackHole $copyLocation/Paradox/Presets/
cp -r ../CloudsData/visualsystems/Paradox/Presets/Vortex4 $copyLocation/Paradox/Presets/
cp -r ../CloudsData/visualsystems/Paradox/Presets/Whitney2 $copyLocation/Paradox/Presets/
mkdir ../../CloudsData/visualsystems/PhotoGlitch
mkdir ../../CloudsData/visualsystems/PhotoGlitch/Presets
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/Code_zoomout $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/FinderWindows $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/FinderWindows_ZoomOut $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/Friends $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/Friends_2 $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/LifeofNumbers_ZoomOut $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/catTest2 $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsData/visualsystems/PhotoGlitch/Presets/glitchTest $copyLocation/PhotoGlitch/Presets/
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/Composits $copyLocation/PhotoGlitch/Composits
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/GLITCH_IMAGES $copyLocation/PhotoGlitch/GLITCH_IMAGES
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/PeopleComposit.png $copyLocation/PhotoGlitch/PeopleComposit.png
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/Presets $copyLocation/PhotoGlitch/Presets
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/snapshots $copyLocation/PhotoGlitch/snapshots
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/sourceImages $copyLocation/PhotoGlitch/sourceImages
cp -r ../CloudsDataIgnored/visualsystems/PhotoGlitch/targetImages $copyLocation/PhotoGlitch/targetImages
mkdir ../../CloudsData/visualsystems/Process11
mkdir ../../CloudsData/visualsystems/Process11/Presets
cp -r ../CloudsData/visualsystems/Process11/Presets/11 $copyLocation/Process11/Presets/
mkdir ../../CloudsData/visualsystems/Process18
mkdir ../../CloudsData/visualsystems/Process18/Presets
cp -r ../CloudsData/visualsystems/Process18/Presets/Pink_Blue $copyLocation/Process18/Presets/
cp -r ../CloudsDataIgnored/visualsystems/Process18/images $copyLocation/Process18/images
mkdir ../../CloudsData/visualsystems/RandomDigits
mkdir ../../CloudsData/visualsystems/RandomDigits/Presets
cp -r ../CloudsData/visualsystems/RandomDigits/Courier.ttf $copyLocation/RandomDigits/Courier.ttf
cp -r ../CloudsData/visualsystems/RandomDigits/frabk.ttf $copyLocation/RandomDigits/frabk.ttf
cp -r ../CloudsData/visualsystems/RandomDigits/NewMedia.ttf $copyLocation/RandomDigits/NewMedia.ttf
cp -r ../CloudsData/visualsystems/RandomDigits/verdana14.ttf $copyLocation/RandomDigits/verdana14.ttf
mkdir ../../CloudsData/visualsystems/RandomDigits2
mkdir ../../CloudsData/visualsystems/RandomDigits2/Presets
cp -r ../CloudsData/visualsystems/RandomDigits2/numbers.png $copyLocation/RandomDigits2/numbers.png
cp -r ../CloudsData/visualsystems/RandomDigits2/Presets/Matrix_3_slower $copyLocation/RandomDigits2/Presets/
cp -r ../CloudsData/visualsystems/RandomDigits2/Presets/Matrix_4_dense $copyLocation/RandomDigits2/Presets/
cp -r ../CloudsData/visualsystems/RandomDigits2/Presets/Matrix_5 $copyLocation/RandomDigits2/Presets/
cp -r ../CloudsData/visualsystems/RandomDigits2/Presets/Matrix_5_dense $copyLocation/RandomDigits2/Presets/
cp -r ../CloudsData/visualsystems/RandomDigits2/Presets/NoiseField $copyLocation/RandomDigits2/Presets/
mkdir ../../CloudsData/visualsystems/Replicator
mkdir ../../CloudsData/visualsystems/Replicator/Presets
cp -r ../CloudsData/visualsystems/Replicator/Presets/Replicator $copyLocation/Replicator/Presets/
cp -r ../CloudsData/visualsystems/Replicator/Presets/ReplicatorOC $copyLocation/Replicator/Presets/
cp -r ../CloudsData/visualsystems/Replicator/Presets/Replicator_echoVortex $copyLocation/Replicator/Presets/
cp -r ../CloudsData/visualsystems/Replicator/Presets/Replicator_organ $copyLocation/Replicator/Presets/
mkdir ../../CloudsData/visualsystems/RGBD
mkdir ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/shaders $copyLocation/RGBD/shaders
cp -r ../CloudsData/visualsystems/RGBD/snapshots $copyLocation/RGBD/snapshots
mkdir ../../CloudsData/visualsystems/RGBDVideo
mkdir ../../CloudsData/visualsystems/RGBDVideo/Presets
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/AIC_Kids $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/Eyebeam_spinningDude $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_1 $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_1OC $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_2_omer $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_2_omerOC $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_3 $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_3OC $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_4 $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ITP_5 $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/KoblinSubway_medium $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/Presets/ShinyLab $copyLocation/RGBDVideo/Presets/
cp -r ../CloudsData/visualsystems/RGBDVideo/shaders $copyLocation/RGBDVideo/shaders
mkdir ../../CloudsData/visualsystems/Ripples
mkdir ../../CloudsData/visualsystems/Ripples/Presets
cp -r ../CloudsData/visualsystems/Ripples/Presets/FilmNoir $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/Presets/PlatinumSunrise $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/Presets/Reverberations $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/Presets/Reverberations1 $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/Presets/Reverberations2_minor $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/Presets/Reverberations3_major4 $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/Presets/Violet $copyLocation/Ripples/Presets/
cp -r ../CloudsData/visualsystems/Ripples/shaders $copyLocation/Ripples/shaders
mkdir ../../CloudsData/visualsystems/Rulez
mkdir ../../CloudsData/visualsystems/Rulez/Presets
cp -r ../CloudsData/visualsystems/Rulez/Presets/Ico_rotate $copyLocation/Rulez/Presets/
cp -r ../CloudsData/visualsystems/Rulez/Presets/Lindenmayer1 $copyLocation/Rulez/Presets/
cp -r ../CloudsData/visualsystems/Rulez/rules $copyLocation/Rulez/rules
cp -r ../CloudsData/visualsystems/Rulez/shaders $copyLocation/Rulez/shaders
mkdir ../../CloudsData/visualsystems/Satoruhiga
mkdir ../../CloudsData/visualsystems/Satoruhiga/Presets
cp -r ../CloudsData/visualsystems/Satoruhiga/Presets/basic1 $copyLocation/Satoruhiga/Presets/
cp -r ../CloudsData/visualsystems/Satoruhiga/Presets/confettistorm $copyLocation/Satoruhiga/Presets/
cp -r ../CloudsData/visualsystems/Satoruhiga/Presets/flamencodancers $copyLocation/Satoruhiga/Presets/
cp -r ../CloudsData/visualsystems/Satoruhiga/Presets/plasticbeast $copyLocation/Satoruhiga/Presets/
cp -r ../CloudsData/visualsystems/Satoruhiga/Presets/stuff $copyLocation/Satoruhiga/Presets/
mkdir ../../CloudsData/visualsystems/Schlabberbox
mkdir ../../CloudsData/visualsystems/Schlabberbox/Presets
cp -r ../CloudsData/visualsystems/Schlabberbox/box.ply $copyLocation/Schlabberbox/box.ply
mkdir ../../CloudsData/visualsystems/Scrape
mkdir ../../CloudsData/visualsystems/Scrape/Presets
mkdir ../../CloudsData/visualsystems/Swim
mkdir ../../CloudsData/visualsystems/Swim/Presets
cp -r ../CloudsData/visualsystems/Swim/images $copyLocation/Swim/images
cp -r ../CloudsData/visualsystems/Swim/models $copyLocation/Swim/models
cp -r ../CloudsData/visualsystems/Swim/Presets/BabyBarbican2014 $copyLocation/Swim/Presets/
cp -r ../CloudsData/visualsystems/Swim/Presets/BaybircarnTwooo $copyLocation/Swim/Presets/
cp -r ../CloudsData/visualsystems/Swim/seed $copyLocation/Swim/seed
cp -r ../CloudsData/visualsystems/Swim/shaders $copyLocation/Swim/shaders
mkdir ../../CloudsData/visualsystems/Terrain
mkdir ../../CloudsData/visualsystems/Terrain/Presets
cp -r ../CloudsData/visualsystems/Terrain/Presets/GraySandbox $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/JM_DrawGoo_rotation $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/JM_Terraformer $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/J_BlackMountains $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/M_DrawGoo $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/M_LandscapeTV $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/M_LiquidTerrain_rotation $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/M_Terraformer $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/Presets/PurpleSandbox $copyLocation/Terrain/Presets/
cp -r ../CloudsData/visualsystems/Terrain/shaders $copyLocation/Terrain/shaders
mkdir ../../CloudsData/visualsystems/TunnelDrawing
mkdir ../../CloudsData/visualsystems/TunnelDrawing/Presets
cp -r ../CloudsData/visualsystems/TunnelDrawing/Presets/BlueWireframe $copyLocation/TunnelDrawing/Presets/
cp -r ../CloudsData/visualsystems/TunnelDrawing/Presets/Ink_1 $copyLocation/TunnelDrawing/Presets/
cp -r ../CloudsData/visualsystems/TunnelDrawing/Presets/Ink_2 $copyLocation/TunnelDrawing/Presets/
cp -r ../CloudsData/visualsystems/TunnelDrawing/shaders $copyLocation/TunnelDrawing/shaders
mkdir ../../CloudsData/visualsystems/Twitter
mkdir ../../CloudsData/visualsystems/Twitter/Presets
cp -r ../CloudsData/visualsystems/Twitter/avatars $copyLocation/Twitter/avatars
cp -r ../CloudsData/visualsystems/Twitter/fonts $copyLocation/Twitter/fonts
cp -r ../CloudsData/visualsystems/Twitter/graphs $copyLocation/Twitter/graphs
cp -r ../CloudsData/visualsystems/Twitter/GraphsInsightInput $copyLocation/Twitter/GraphsInsightInput
cp -r ../CloudsData/visualsystems/Twitter/images $copyLocation/Twitter/images
cp -r ../CloudsData/visualsystems/Twitter/Presets/CLOUDS $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/CLOUDS_2 $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/HeartOftheInternet $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/InsideTheNet $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/JM_TheCloud_b $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/JM_TheCloud_c_Massive $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/JM_basic $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/TweetDeck_NameCloud_Lines $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/jonathanAndSuryasBogusAdventure $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/latestDeckWithIcons $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/nameCloudWithLinesFlickering $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/nameCloudsWithLinesFlickering $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/nameSprites $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/stars_handles $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/Presets/tweetDeck $copyLocation/Twitter/Presets/
cp -r ../CloudsData/visualsystems/Twitter/RockwellStd.otf $copyLocation/Twitter/RockwellStd.otf
cp -r ../CloudsData/visualsystems/Twitter/shaders $copyLocation/Twitter/shaders
cp -r ../CloudsData/visualsystems/Twitter/tweetDeckMenu.png $copyLocation/Twitter/tweetDeckMenu.png
cp -r ../CloudsData/visualsystems/Twitter/twitternames.txt $copyLocation/Twitter/twitternames.txt
cp -r ../CloudsData/visualsystems/Twitter/twitterVSNewData $copyLocation/Twitter/twitterVSNewData
cp -r ../CloudsDataIgnored/visualsystems/Twitter/twitter.csv $copyLocation/Twitter/twitter.csv
mkdir ../../CloudsData/visualsystems/Uextrude
mkdir ../../CloudsData/visualsystems/Uextrude/Presets
cp -r ../CloudsData/visualsystems/Uextrude/shaders $copyLocation/Uextrude/shaders
mkdir ../../CloudsData/visualsystems/VectorFlow
mkdir ../../CloudsData/visualsystems/VectorFlow/Presets
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/ReasLike_1 $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/ReasLike_2 $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/ReasLike_3 $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/SwiftWaves_Field $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/Vortex $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/WindField $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/trippywaves $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/trippywavesBlur $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/Presets/trippywavesFast $copyLocation/VectorFlow/Presets/
cp -r ../CloudsData/visualsystems/VectorFlow/shaders $copyLocation/VectorFlow/shaders
mkdir ../../CloudsData/visualsystems/VerletForm
mkdir ../../CloudsData/visualsystems/VerletForm/Presets
cp -r ../CloudsData/visualsystems/VerletForm/Palettes $copyLocation/VerletForm/Palettes
cp -r ../CloudsData/visualsystems/VerletForm/Presets/JG_does_marius $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/JG_does_marius2 $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/JG_does_marius3 $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/JG_does_marius3OC $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/MWDark $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/MWDarkOC $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/MWLORES $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/MWLORES_OC $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/Tower $copyLocation/VerletForm/Presets/
cp -r ../CloudsData/visualsystems/VerletForm/Presets/mw01 $copyLocation/VerletForm/Presets/
mkdir ../../CloudsData/visualsystems/Vision
mkdir ../../CloudsData/visualsystems/Vision/Presets
cp -r ../CloudsData/visualsystems/Vision/heatMapShader.frag $copyLocation/Vision/heatMapShader.frag
cp -r ../CloudsData/visualsystems/Vision/heatMapShader.vert $copyLocation/Vision/heatMapShader.vert
cp -r ../CloudsData/visualsystems/Vision/images $copyLocation/Vision/images
cp -r ../CloudsData/visualsystems/Vision/Presets/BridgePreset $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/GreenPoint_Contour_extreme $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/Soho_Boxes $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/Soho_HeatMap $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/Tokyo1_Blob $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/TokyoCrossing_flow $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/TokyoEscalator $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/TokyoEscalatorFlow $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/UnionSqGhost $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/flowDancer $copyLocation/Vision/Presets/
cp -r ../CloudsData/visualsystems/Vision/Presets/tokyoCrossingHeatMap $copyLocation/Vision/Presets/
mkdir ../../CloudsData/visualsystems/Visuals
mkdir ../../CloudsData/visualsystems/Visuals/Presets
cp -r ../CloudsData/visualsystems/Visuals/visualslist.txt $copyLocation/Visuals/visualslist.txt
mkdir ../../CloudsData/visualsystems/Voro
mkdir ../../CloudsData/visualsystems/Voro/Presets
cp -r ../CloudsData/visualsystems/Voro/images $copyLocation/Voro/images
cp -r ../CloudsData/visualsystems/Voro/Presets/BluePrint_a $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/Dream_Crystal1 $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/Parametric_a_flickery $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/Parametric_b $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/Parametric_c_wires $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/Parametric_d_wires $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/SpaceBoulders_B $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/crystal1 $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/Presets/g_sloBurn $copyLocation/Voro/Presets/
cp -r ../CloudsData/visualsystems/Voro/shaders $copyLocation/Voro/shaders
mkdir ../../CloudsData/visualsystems/WebHistory
mkdir ../../CloudsData/visualsystems/WebHistory/Presets
cp -r ../CloudsData/visualsystems/WebHistory/Andale Mono.ttf $copyLocation/WebHistory/Andale Mono.ttf
cp -r ../CloudsData/visualsystems/WebHistory/Presets/AutoSpiderShell $copyLocation/WebHistory/Presets/
cp -r ../CloudsData/visualsystems/WebHistory/Presets/Hazy_wide $copyLocation/WebHistory/Presets/
cp -r ../CloudsData/visualsystems/WebHistory/RockwellStd.otf $copyLocation/WebHistory/RockwellStd.otf
cp -r ../CloudsData/visualsystems/WebHistory/SampleChromeHistory $copyLocation/WebHistory/SampleChromeHistory
mkdir ../../CloudsData/visualsystems/World
mkdir ../../CloudsData/visualsystems/World/Presets
cp -r ../CloudsData/visualsystems/World/airports.txt $copyLocation/World/airports.txt
cp -r ../CloudsData/visualsystems/World/CamSettings_DIST.xml $copyLocation/World/CamSettings_DIST.xml
cp -r ../CloudsData/visualsystems/World/CamSettings_FOV.xml $copyLocation/World/CamSettings_FOV.xml
cp -r ../CloudsData/visualsystems/World/cities.txt $copyLocation/World/cities.txt
cp -r ../CloudsData/visualsystems/World/constelations.txt $copyLocation/World/constelations.txt
cp -r ../CloudsData/visualsystems/World/images $copyLocation/World/images
cp -r ../CloudsData/visualsystems/World/loc.txt $copyLocation/World/loc.txt
cp -r ../CloudsData/visualsystems/World/Presets/Arcs $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/Arcs2 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/Atomic $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/Atomic2 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/AtomicOC $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/DataWorld1 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/Data_globe2 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/GeoData $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/Social_Network2 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/Social_NetworkOC $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/SpaceJunkOC $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/g_Intro_03 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/Presets/jm_data1 $copyLocation/World/Presets/
cp -r ../CloudsData/visualsystems/World/shaders $copyLocation/World/shaders
cp -r ../CloudsData/visualsystems/World/simple-borders.txt $copyLocation/World/simple-borders.txt
cp -r ../CloudsData/visualsystems/World/simple-cities.txt $copyLocation/World/simple-cities.txt
cp -r ../CloudsData/visualsystems/World/simple-coast.txt $copyLocation/World/simple-coast.txt
cp -r ../CloudsData/visualsystems/World/simple-rivers.txt $copyLocation/World/simple-rivers.txt
mkdir ../../CloudsData/visualsystems/WormHole
mkdir ../../CloudsData/visualsystems/WormHole/Presets
cp -r ../CloudsData/visualsystems/WormHole/cameraPaths $copyLocation/WormHole/cameraPaths
cp -r ../CloudsData/visualsystems/WormHole/Presets/Aminos_2 $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/Aminos_3a $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/OrganicTunnel_2_White $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/OrganicTunnel_2_White_Faster $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/OrganicTunnel_2_fast $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/OrganicTunnel_2_medium $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/OrganicTunnel_2_slow $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelOrganix_10_xray $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelv2_01_white $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelv2_03_BrownXray $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelv2_04_white $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelv2_05_xrayGray $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelv2_06_transparent_blue $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/gTunnelv3_03_white $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/lightAndFog $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/Presets/lightAndFog_2 $copyLocation/WormHole/Presets/
cp -r ../CloudsData/visualsystems/WormHole/shaders $copyLocation/WormHole/shaders
cp -r ../CloudsDataIgnored/visualsystems/WormHole/cameraPaths $copyLocation/WormHole/cameraPaths
cp -r ../CloudsDataIgnored/visualsystems/WormHole/models_binary $copyLocation/WormHole/models_binary
mkdir ../../CloudsData/visualsystems/Xstatic
mkdir ../../CloudsData/visualsystems/Xstatic/Presets
cp -r ../CloudsData/visualsystems/Xstatic/Presets/BigPong3_lavendar $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/BigPong3_yellow $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/BouncingBalls $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/BouncingBalls_TumblingBox $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/FastPhysics2_TumblingBox $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/FastPhysics2_TumblingBoxOC $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/FastPhysics_TumblingBox $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/Neutrinos_1 $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/Neutrinos_2 $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/Presets/Neutrinos_3 $copyLocation/Xstatic/Presets/
cp -r ../CloudsData/visualsystems/Xstatic/shaders $copyLocation/Xstatic/shaders
cp -r ../CloudsData/visualsystems/Xstatic/spriteImages $copyLocation/Xstatic/spriteImages
mkdir ../../CloudsData/visualsystems/YellowTail
mkdir ../../CloudsData/visualsystems/YellowTail/Presets
cp -r ../CloudsData/visualsystems/YellowTail/Presets/YellowTail $copyLocation/YellowTail/Presets/
cp -r ../CloudsData/visualsystems/YellowTail/Presets/YellowTailOC $copyLocation/YellowTail/Presets/
cp -r ../CloudsData/visualsystems/YellowTail/Presets/YellowTail_AfterDark $copyLocation/YellowTail/Presets/
cp -r ../CloudsData/visualsystems/YellowTail/strokes $copyLocation/YellowTail/strokes
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/FollowTraverse_OculusHD_fast ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/FollowTraverse_OculusHD_pretty ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/FollowTraverse_Screen_fast ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/FollowTraverse_Screen_pretty ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/NavigationInterlude_Screen ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/TopicResearch_fast ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_ClusterMap/Presets/TopicResearch_pretty ../../CloudsData/visualsystems/_ClusterMap/Presets
cp -r ../CloudsData/visualsystems/_Intro/Presets/TunnelWarp_pretty ../../CloudsData/visualsystems/_Intro/Presets
cp -r ../CloudsData/visualsystems/_Intro/Presets/TunnelWarp_fast ../../CloudsData/visualsystems/_Intro/Presets
cp -r ../CloudsData/visualsystems/_Intro/Presets/Oculus_pretty ../../CloudsData/visualsystems/_Intro/Presets
cp -r ../CloudsData/visualsystems/_Intro/Presets/Oculus_fast ../../CloudsData/visualsystems/_Intro/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_ACT1_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_ACT1_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_ACT2_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_ACT2_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_ACT3_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_ACT3_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_BASE_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_BASE_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_ACT1_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_ACT1_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_ACT2_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_ACT2_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_ACT3_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_ACT3_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_BASE_fast ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/RGBD/Presets/RGBD_OC_BASE_pretty ../../CloudsData/visualsystems/RGBD/Presets
cp -r ../CloudsData/visualsystems/Twitter/Presets/ResearchPeople_pretty ../../CloudsData/visualsystems/Twitter/Presets
cp -r ../CloudsData/visualsystems/Twitter/Presets/ResearchPeople_fast ../../CloudsData/visualsystems/Twitter/Presets
cp -r ../CloudsData/visualsystems/Visuals/Presets/VisualPicker_pretty ../../CloudsData/visualsystems/Visuals/Presets
cp -r ../CloudsData/visualsystems/Visuals/Presets/VisualPicker_fast ../../CloudsData/visualsystems/Visuals/Presets
