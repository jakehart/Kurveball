# Release Roadmap
This summarizes all the features and fixes that are needed in order to release Kurveball.

## Documentation
mini-clip for README  
Explainer video  

## Unreal Demo
Example animation blueprint
RespectCollision bulletproofing
UE networking adapter  
"Create movement mechanic" menu item  
Movement line/subway map  
bugfix: falling through the floor at low framerates
	this is due to the sensor check for gravity, which is hacky. remove USensorComponent

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
Naming consistency: StartVelocityCurve etc. all take mechanics and should be named StartCurveMechanic etc., preface Unreal assets with the commonly accepted two-letter prefixes, replace all Id with ID
pretty printing - find a satisfactory tool for this