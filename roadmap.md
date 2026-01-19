# Release Roadmap
This summarizes all the features and fixes that are needed in order to release CurveLib.

## Unreal Demo
Fix behavior on slopes  
Climbing mechanic  
Fix killplane logic  
Networked play using UE networking  
"Create movement mechanic" menu item  
Move demo to Examples folder (while somehow maintaining ability to see CurveLib -- .lib may be needed here?)  
Movement line  
Other genres  

## Playback
Cancel on collision  
Speed sync

## BezierCurveSegment: Pack-in curve implementation
2D and 3D samplers  
Sample Bezier by arc distance  
Sample Bezier by x (plug desired x to get t, then use t to get y)  
Should be able to share points between adjacent segments. Either with refs, or by making an optimized BezierCurve and ditching the segments 

## Networking
Spline recording (Construct Catmull-Rom from network breadcrumb circular buffer)  

## Curve Editor
Bug: does not support Windows display scales other than 100% -- check the event to see if there's scale info in it  
Load/save functionality  
Support for multiple segments  
Visualize playback (e.g. using a 1D or 2D point)  
Show distance travelled on mouseover
	Since this depends on the top speed of the mechanic, would it make sense for CurveEditor to be aware of MovementMechanic?

## Housekeeping
Naming consistency  

## Documentation
NAME the library  
Explainer video  
README.md with example code and clip  