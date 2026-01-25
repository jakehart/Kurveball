#pragma once
#include "AreaAccumulator.h"
#include "BezierCurve.h"
#include "Vector2.h"

struct ImVec4;

namespace CurveLib
{
	class CurveEditor
	{
	public:
		CurveEditor();
		~CurveEditor();
		CurveEditor(const CurveEditor&) = delete;
		CurveEditor(CurveEditor&&) = delete;

		// Returns true if the program should keep running, otherwise false
		bool Tick();

	private:
		// Returns true on success
		bool Init();

		void DrawGUI();
		void DrawIntegration();
		// Blocking function to open a Windows save dialog and save a curve as a .cvb binary file.
		void DrawSaveDialog();
		// Blocking function to open a Windows open dialog
		void DrawOpenDialog();

		static const ImVec4 sClearColor;
		
		BezierCurve<Double2> mDefaultCurve;
		AreaAccumulator mIntegrationAccumulator;

        bool mIsIntegrationEnabled = true;
        bool mIsLookupTableDrawn = true;
        // Signed to match the IMGUI input box function
        int32_t mNumSamplesPerSegment = 32;
	};
}