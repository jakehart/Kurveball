# Release Roadmap
This summarizes all the features and fixes that are needed in order to release Kurveball.

## Documentation
Explainer video  

## Unreal Demo
bugfix: falling through the floor at low framerates
	this is due to the sensor check for gravity, which is hacky. remove USensorComponent
Fix "Create movement mechanic" menu item (move to extension)  
Movement line/subway map  

## BezierCurveSegment: Pack-in curve implementation
Should be able to share points between adjacent segments. Either with refs, or by making an optimized BezierCurve and ditching the segments  
Sample Bezier by arc length
	3D distance-to-t lookup populated "backwards"
Point reordering  

## Networking
Spline recording (Construct Catmull-Rom from network breadcrumb circular buffer)  

## Curve Editor
Integration - Show area under curve and distance travelled at each x on mouseover - use mechanic data
Bug: does not support Windows display scales other than 100% -- bug confirmed present in other imgui applications, investigate imgui fix
Visualize playback (e.g. using a 1D or 2D point)  

## Housekeeping
Naming consistency: preface Unreal assets with the commonly accepted two-letter prefixes   