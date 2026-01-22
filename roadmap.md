# Release Roadmap
This summarizes all the features and fixes that are needed in order to release CurveLib.

## Unreal Demo
Some solution for slope traversability. Samnple x of a "slope curve" based on slope angle? and then multiply with the horizontal curves 
Fix grapple  
Stretch spline to height and destination  
Networked play using UE networking  
"Create movement mechanic" menu item  
Movement line/subway map  

## Playback
Cancel on collision setting/action -- since the curve sim doesn't know directly about collisions, would need to add a ReportCollision API function  
Speed sync  

## BezierCurveSegment: Pack-in curve implementation
Sample Bezier by arc distance  
Sample Bezier by x (plug desired x to get t, then use t to get y)  
Should be able to share points between adjacent segments. Either with refs, or by making an optimized BezierCurve and ditching the segments  
Point reordering  

## Networking
Spline recording (Construct Catmull-Rom from network breadcrumb circular buffer)  

## Curve Editor
Bug: does not support Windows display scales other than 100% -- check the event to see if there's scale info in it  
Load/save buttons broken?  
Visualize playback (e.g. using a 1D or 2D point)  
Integration - Show area under curve and distance travelled at each x on mouseover
	Since the latter depends on the top speed of the mechanic, would it make sense for CurveEditor to be aware of MovementMechanic?

## Housekeeping
Naming consistency  
pretty printing

## Documentation
NAME the library  
Explainer video  
README.md with example code and clip  