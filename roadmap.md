# v1.0 Roadmap
This summarizes all the features and fixes that are needed in order to to get to the first release of Kurveball (v1.0).

## Core
Curve->curve speed matching and blending

## Unreal Demo
Unreal quickstart video from zero   
bugfix: falling through the floor at low framerates
	this is due to the sensor check for gravity, which is hacky. remove USensorComponent
Fix "Create movement mechanic" menu item (move to extension)  

## BezierCurveSegment: Pack-in curve implementation
Should be able to share points between adjacent segments. Either with refs, or by making an optimized BezierCurve and ditching the segments  
Sample Bezier by arc length
	3D distance-to-t lookup populated "backwards"
Point reordering  

## Housekeeping
Naming consistency: preface Unreal assets with the commonly accepted two-letter prefixes   

# v1.1 Roadmap
Lower priority features that will make it into the next release.

## Godot wrapper

## Networking
Spline recording (Construct Catmull-Rom from network breadcrumb circular buffer)  

## Developer Quality of Life
Movement line/subway map  
UI wrapper to expose same widgets to the user regardless of their UI framework. UI primitives exposed to me would DrawGraph(someArray), DrawCircle, etc. OR just draw directly to a memory buffer and let them render it

## Curve Editor
Integration - Show area under curve and distance travelled at each x on mouseover - use mechanic data
Bug: does not support Windows display scales other than 100% -- bug confirmed present in other imgui applications, investigate imgui fix
Visualize playback (e.g. using a 1D or 2D point)  