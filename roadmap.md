# v1.0 Roadmap
This summarizes all the features and fixes that are needed in order to to get to the first release of Kurveball (v1.0).

## Core
util to reach position using curve instance by setting its mStretchDuration and/or mPlayCount. some of this gap in functionality is already filled by GenerateParabolicSpline, but that doesn't cover the base case of e.g. walking somewhere, and we shouldn't need splines just to reach a destination.  
bugfix: no velocity or direction output on the first tick (since there's no accumulator area yet). solution is to calculate them directly  

## Unreal
Early out from jump when button is released
All-in-one mechanic/curve editor
Unreal quickstart video from zero  
bugfix: fix coordinate space issue when RespectCollision==true. Could be in InputAxisToVelocityCurve()?
bugfix: very occasionally landing just above or below the floor.
Convert to and from BezierCurveSegment (pack-in curve implementation)  

## BezierCurveSegment: Pack-in curve implementation
Should be able to share points between adjacent segments. Either with refs, or by making an optimized BezierCurve and ditching the segments  
Sample Bezier by arc length
	3D distance-to-t lookup populated "backwards"
Point reordering  
Library of commonly used curves (easing functions and perhaps their derivatives)

# v1.1 Roadmap
Lower priority features that will make it into the next release.

## Core
Logging wrapper lambda to expose internal logs to arbitrary engines

## Unreal
Add climb mechanics

## Godot wrapper

## Networking
Spline recording (Construct Catmull-Rom from network breadcrumb circular buffer)  

## Developer Quality of Life
Movement line/subway map  
UI wrapper to expose same widgets to the user regardless of their UI framework. UI primitives exposed to me would make it easier to create debug UI across the board. DrawGraph(someArray), DrawCircle, etc. This is superior to rendering everything to a texture myself because the wrapped function would take care of the annoying stuff

## Curve Editor
Integration - Show area under curve and distance travelled at each x on mouseover - use mechanic data
Bug: does not support Windows display scales other than 100% -- bug confirmed present in other imgui applications, investigate imgui fix
Visualize playback (e.g. using a 1D or 2D point)  