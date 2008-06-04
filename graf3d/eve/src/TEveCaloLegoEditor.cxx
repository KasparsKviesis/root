// @(#)root/eve:$Id$
// Author: Matevz Tadel 2007

/*************************************************************************
 * Copyright (C) 1995-2007, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TEveCaloLegoEditor.h"
#include "TEveCalo.h"
#include "TEveGValuators.h"
#include "TGComboBox.h"

#include "TColor.h"
#include "TGColorSelect.h"
#include "TGLabel.h"
#include "TG3DLine.h"

//______________________________________________________________________________
// GUI editor for TEveCaloLego.
//

ClassImp(TEveCaloLegoEditor);

//______________________________________________________________________________
TEveCaloLegoEditor::TEveCaloLegoEditor(const TGWindow *p, Int_t width, Int_t height,
                                       UInt_t options, Pixel_t back) :
   TGedFrame(p, width, height, options | kVerticalFrame, back),
   fM(0),
   fGridColor(0),
   fFontColor(0),
   fPlaneColor(0),
   fTransparency(0),

   fNZSteps(0),

   fBinWidth(0),

   fProjection(0),
   f2DMode(0),
   fBoxMode(0)
{
   // Constructor.

   MakeTitle("TEveCaloLego");

   {
      // grid color
      TGHorizontalFrame* f = new TGHorizontalFrame(this);
      TGLabel* lab = new TGLabel(f, "GridColor:");
      f->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsBottom, 1, 10, 1, 1));

      fGridColor = new TGColorSelect(f, 0, -1);
      f->AddFrame(fGridColor, new TGLayoutHints(kLHintsLeft|kLHintsTop, 3, 1, 0, 1));
      fGridColor->Connect("ColorSelected(Pixel_t)", "TEveCaloLegoEditor", this, "DoGridColor(Pixel_t)");

      AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
   }
   // axis
   {
      // font color
      TGHorizontalFrame* f = new TGHorizontalFrame(this);
      TGLabel* lab = new TGLabel(f, "FontColor:");
      f->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsBottom, 1, 8, 1, 1));

      fFontColor = new TGColorSelect(f, 0, -1);
      f->AddFrame(fFontColor, new TGLayoutHints(kLHintsLeft|kLHintsTop, 3, 1, 0, 1));
      fFontColor->Connect("ColorSelected(Pixel_t)", "TEveCaloLegoEditor", this, "DoFontColor(Pixel_t)");

      AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
   }
   {
      // plane color
      TGHorizontalFrame* f = new TGHorizontalFrame(this);
      TGLabel* lab = new TGLabel(f, "PlaneColor:");
      f->AddFrame(lab, new TGLayoutHints(kLHintsLeft|kLHintsBottom, 1, 1, 1, 1));

      fPlaneColor = new TGColorSelect(f, 0, -1);
      f->AddFrame(fPlaneColor, new TGLayoutHints(kLHintsLeft|kLHintsTop, 3, 1, 0, 1));
      fPlaneColor->Connect("ColorSelected(Pixel_t)", "TEveCaloLegoEditor", this, "DoPlaneColor(Pixel_t)");

      fTransparency = new TGNumberEntry(f, 0., 2, -1,
                                        TGNumberFormat::kNESInteger, TGNumberFormat::kNEANonNegative,
                                        TGNumberFormat::kNELLimitMinMax, 0, 100);
      fTransparency->SetHeight(18);
      fTransparency->GetNumberEntry()->SetToolTipText("Transparency: 0 is opaque, 100 fully transparent.");
      f->AddFrame(fTransparency, new TGLayoutHints(kLHintsLeft, 0, 0, 0, 0));
      fTransparency->Connect("ValueSet(Long_t)","TEveCaloLegoEditor", this, "DoTransparency()");



      AddFrame(f, new TGLayoutHints(kLHintsTop, 1, 1, 1, 0));
   }

   Int_t lw = 80;
   fNZSteps = new TEveGValuator(this, "NZTickMarks:", 90, 0);
   fNZSteps->SetLabelWidth(lw);
   fNZSteps->SetNELength(5);
   fNZSteps->SetShowSlider(kFALSE);
   fNZSteps->Build();
   fNZSteps->SetLimits(1, 20);
   fNZSteps->SetToolTip("Number of labels along the Z axis.");
   fNZSteps->Connect("ValueSet(Double_t)", "TEveCaloLegoEditor", this, "DoNZSteps()");
   AddFrame(fNZSteps, new TGLayoutHints(kLHintsTop, 4, 2, 1, 2));

   fBinWidth = new TEveGValuator(this, "BinWidth:", 90, 0);
   fBinWidth->SetLabelWidth(lw);
   fBinWidth->SetNELength(5);
   fBinWidth->SetShowSlider(kFALSE);
   fBinWidth->Build();
   fBinWidth->SetLimits(1, 20);
   fBinWidth->SetToolTip("Number of labels along the Z axis.");
   fBinWidth->Connect("ValueSet(Double_t)", "TEveCaloLegoEditor", this, "DoBinWidth()");
   AddFrame(fBinWidth, new TGLayoutHints(kLHintsTop, 4, 2, 1, 2));

   fProjection = MakeLabeledCombo("Project:", 1);
   fProjection->AddEntry("Auto", TEveCaloLego::kAuto);
   fProjection->AddEntry("3D", TEveCaloLego::k3D);
   fProjection->AddEntry("2D", TEveCaloLego::k2D);
   fProjection->Connect("Selected(Int_t)", "TEveCaloLegoEditor", this, "DoProjection()");

   f2DMode = MakeLabeledCombo("2DMode:", 4);
   f2DMode->AddEntry("ValColor", TEveCaloLego::kValColor);
   f2DMode->AddEntry("ValSize",  TEveCaloLego::kValSize);
   f2DMode->Connect("Selected(Int_t)", "TEveCaloLegoEditor", this, "Do2DMode()");

   fBoxMode = MakeLabeledCombo("Box:", 4);
   fBoxMode->AddEntry("None", TEveCaloLego::kNone);
   fBoxMode->AddEntry("Back",  TEveCaloLego::kBack);
   fBoxMode->AddEntry("FrontBack",  TEveCaloLego::kFrontBack);
   fBoxMode->Connect("Selected(Int_t)", "TEveCaloLegoEditor", this, "DoBoxMode()");
}

//______________________________________________________________________________
TGComboBox* TEveCaloLegoEditor::MakeLabeledCombo(const char* name, Int_t off)
{
   // Helper function. Creates TGComboBox with fixed size TGLabel.

   UInt_t labelW = 60;
   UInt_t labelH = 20;
   TGHorizontalFrame* hf = new TGHorizontalFrame(this);
   // label
   TGCompositeFrame *labfr = new TGHorizontalFrame(hf, labelW, labelH, kFixedSize);
   TGLabel* label = new TGLabel(labfr, name);
   labfr->AddFrame(label, new TGLayoutHints(kLHintsLeft  | kLHintsBottom));
   hf->AddFrame(labfr, new TGLayoutHints(kLHintsLeft));
   // combo
   TGLayoutHints*  clh =  new TGLayoutHints(kLHintsLeft, 0,0,0,0);
   TGComboBox* combo = new TGComboBox(hf);
   combo->Resize(90, 20);
   hf->AddFrame(combo, clh);

   AddFrame(hf, new TGLayoutHints(kLHintsTop, 4, 1, 1, off));
   return combo;
}

//______________________________________________________________________________
void TEveCaloLegoEditor::SetModel(TObject* obj)
{
   // Set model object.

   fM = dynamic_cast<TEveCaloLego*>(obj);
   fGridColor->SetColor(TColor::Number2Pixel(fM->GetGridColor()), kFALSE);
   fFontColor->SetColor(TColor::Number2Pixel(fM->GetFontColor()), kFALSE);

   fPlaneColor->SetColor(TColor::Number2Pixel(fM->GetPlaneColor()), kFALSE);
   fTransparency->SetNumber(fM->GetPlaneTransparency());

   fNZSteps->SetValue(fM->GetNZSteps());
   fBinWidth->SetValue(fM->GetBinWidth());

   fProjection->Select(fM->GetProjection(), kFALSE);
   f2DMode->Select(fM->Get2DMode(), kFALSE);
   fBoxMode->Select(fM->GetBoxMode(), kFALSE);
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoGridColor(Pixel_t pixel)
{
   // Slot for GridColor.

   fM->SetGridColor(Color_t(TColor::GetColor(pixel)));
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoFontColor(Pixel_t pixel)
{
   // Slot for FontColor.

   fM->SetFontColor(Color_t(TColor::GetColor(pixel)));
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoPlaneColor(Pixel_t pixel)
{
   // Slot for PlaneColor.

   fM->SetPlaneColor(Color_t(TColor::GetColor(pixel)));
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoNZSteps()
{
   // Slot for NZStep.

   fM->SetNZSteps((Int_t)fNZSteps->GetValue());
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoBinWidth()
{
   // Slot for BinWidth.

   fM->SetBinWidth((Int_t)fBinWidth->GetValue());
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoProjection()
{
   // Slot for projection.

   fM->SetProjection((TEveCaloLego::EProjection_e)fProjection->GetSelected());
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::Do2DMode()
{
   // Slot for projection.

   fM->Set2DMode((TEveCaloLego::E2DMode_e)f2DMode->GetSelected());
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoBoxMode()
{
   // Slot for projection.

   fM->SetBoxMode((TEveCaloLego::EBoxMode_e)fBoxMode->GetSelected());
   Update();
}

//______________________________________________________________________________
void TEveCaloLegoEditor::DoTransparency()
{
   // Slot for Transparency.

   fM->SetPlaneTransparency((UChar_t)(fTransparency->GetNumber()));
   Update();
}
