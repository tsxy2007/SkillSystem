// Fill out your copyright notice in the Description page of Project Settings.


#include "Details/FSkillTreeDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "IPropertyUtilities.h"
#include "Layout/Margin.h"
#include "Misc/Attribute.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SlateOptMacros.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Widgets/SWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SComboButton.h"

#define LOCTEXT_NAMESPACE "SkillTreeDetails"
TSharedRef<IDetailCustomization> FSkillTreeDetails::MakeInstance()
{
	return MakeShareable(new FSkillTreeDetails());
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void FSkillTreeDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& MyCategory = DetailBuilder.EditCategory("CategoryName", FText::FromString("Extra Info"), ECategoryPriority::Default);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION



bool FSkillTreeDetails::IsEditorOnlyObject() const
{
	return false;
}

void FSkillTreeDetails::InitPropertyValues()
{

}

#undef LOCTEXT_NAMESPACE