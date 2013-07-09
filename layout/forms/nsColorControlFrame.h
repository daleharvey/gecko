/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsColorControlFrame_h___
#define nsColorControlFrame_h___

#include "nsBlockFrame.h"
#include "nsCOMPtr.h"
#include "nsIAnonymousContentCreator.h"

// Class which implements the input type=color

class nsColorControlFrame MOZ_FINAL : public nsBlockFrame,
                                      public nsIAnonymousContentCreator
{
public:
  friend nsIFrame* NS_NewColorControlFrame(nsIPresShell* aPresShell,
                                           nsStyleContext* aContext);

  virtual void DestroyFrom(nsIFrame* aDestructRoot) MOZ_OVERRIDE;

  NS_DECL_FRAMEARENA_HELPERS
  NS_DECL_QUERYFRAME

  virtual nsIAtom* GetType() const MOZ_OVERRIDE;

#ifdef DEBUG
  NS_IMETHOD GetFrameName(nsAString& aResult) const;
#endif

  // nsIAnonymousContentCreator
  virtual nsresult CreateAnonymousContent(nsTArray<ContentInfo>& aElements) MOZ_OVERRIDE;
  virtual void AppendAnonymousContentTo(nsBaseContentList& aElements,
                                        uint32_t aFilter) MOZ_OVERRIDE;

  // nsIFrame
  NS_IMETHOD AttributeChanged(int32_t  aNameSpaceID,
                              nsIAtom* aAttribute,
                              int32_t  aModType) MOZ_OVERRIDE;
  virtual bool IsLeaf() const MOZ_OVERRIDE { return true; }
  virtual nsIFrame* GetContentInsertionFrame() MOZ_OVERRIDE;

  virtual void BuildDisplayList(nsDisplayListBuilder*   aBuilder,
                                const nsRect&           aDirtyRect,
                                const nsDisplayListSet& aLists) MOZ_OVERRIDE;

  virtual bool IsFrameOfType(uint32_t aFlags) const MOZ_OVERRIDE
  {
    return nsBlockFrame::IsFrameOfType(aFlags &
      ~(nsIFrame::eReplaced | nsIFrame::eReplacedContainsBlock));
  }

private:
  nsColorControlFrame(nsStyleContext* aContext);

  // Update the color swatch
  nsresult UpdateColor();

  nsCOMPtr<nsIContent> mColorContent;
};


#endif // nsColorControlFrame_h___
