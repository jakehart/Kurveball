#pragma once
#include "WindowState.h"
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
		void DrawGUI();
		// Blocking function to open a Windows save dialog and save a curve as a .cvb binary file.
		void DrawSaveDialog();
		// Blocking function to open a Windows open dialog
		void DrawOpenDialog();


		static const ImVec4 sClearColor;
		WindowState mWindowState;

		// TODO: Encapsulate these globals properly in application state
		BezierCurve<Double2> defaultCurve;
		bool gDrawIntegration = true;
	};
}