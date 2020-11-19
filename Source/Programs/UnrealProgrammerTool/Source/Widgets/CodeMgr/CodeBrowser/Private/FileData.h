#pragma once
#include "CoreMinimal.h"


/**
 * A struct to hold important information about an assets found by the Asset Registry
 * This struct is transient and should never be serialized
 */
struct FFileData
{
public:

	/** The object path for the asset in the form PackageName.AssetName. Only top level objects in a package can have AssetData */
		FName ObjectPath;
	/** The name of the package in which the asset is found, this is the full long package name such as /Game/Path/Package */
		FName PackageName;
	/** The path to the package in which the asset is found, this is /Game/Path with the Package stripped off */
		FName PackagePath;
	/** The name of the asset without the package */
		FName AssetName;
	/** The name of the asset's class */
		FName AssetClass;
	/** The map of values for properties that were marked AssetRegistrySearchable or added by GetAssetRegistryTags */
	//FFileDataTagMapSharedView TagsAndValues;
	/** The IDs of the chunks this asset is located in for streaming install.  Empty if not assigned to a chunk */
	TArray<int32> ChunkIDs;
	/** Asset package flags */
	uint32 PackageFlags;

public:
	/** Default constructor */
	FFileData()
	{}

	/** Constructor */
	FFileData(FName InPackageName, FName InPackagePath, FName InAssetName, FName InAssetClass, FFileDataTagMap InTags = FFileDataTagMap(), TArray<int32> InChunkIDs = TArray<int32>(), uint32 InPackageFlags = 0)
		: PackageName(InPackageName)
		, PackagePath(InPackagePath)
		, AssetName(InAssetName)
		, AssetClass(InAssetClass)
		, TagsAndValues(MoveTemp(InTags))
		, ChunkIDs(MoveTemp(InChunkIDs))
		, PackageFlags(InPackageFlags)
	{
		FString ObjectPathStr = PackageName.ToString() + TEXT(".");

		ObjectPathStr += AssetName.ToString();

		ObjectPath = FName(*ObjectPathStr);
	}

	/** Constructor taking a UObject. By default trying to create one for a blueprint class will create one for the UBlueprint instead, but this can be overridden */
	FFileData(const UObject* InAsset, bool bAllowBlueprintClass = false)
	{
		if (InAsset != nullptr)
		{
			const UClass* InClass = Cast<UClass>(InAsset);
			if (InClass && InClass->ClassGeneratedBy && !bAllowBlueprintClass)
			{
				// For Blueprints, the AssetData refers to the UBlueprint and not the UBlueprintGeneratedClass
				InAsset = InClass->ClassGeneratedBy;
			}

			const UPackage* Outermost = InAsset->GetOutermost();
			const UObject* Outer = InAsset->GetOuter();

			PackageName = Outermost->GetFName();
			PackagePath = FName(*FPackageName::GetLongPackagePath(Outermost->GetName()));
			AssetName = InAsset->GetFName();
			AssetClass = InAsset->GetClass()->GetFName();
			ObjectPath = FName(*InAsset->GetPathName());

			TArray<UObject::FAssetRegistryTag> ObjectTags;
			InAsset->GetAssetRegistryTags(ObjectTags);

			FFileDataTagMap NewTagsAndValues;
			for (UObject::FAssetRegistryTag& AssetRegistryTag : ObjectTags)
			{
				if (AssetRegistryTag.Name != NAME_None && !AssetRegistryTag.Value.IsEmpty())
				{
					// Don't add empty tags
					NewTagsAndValues.Add(AssetRegistryTag.Name, AssetRegistryTag.Value);
				}
			}

			TagsAndValues = FFileDataTagMapSharedView(MoveTemp(NewTagsAndValues));
			ChunkIDs = Outermost->GetChunkIDs();
			PackageFlags = Outermost->GetPackageFlags();
		}
	}

	/** FFileDatas are equal if their object paths match */
	bool operator==(const FFileData& Other) const
	{
		return ObjectPath == Other.ObjectPath;
	}

	bool operator!=(const FFileData& Other) const
	{
		return ObjectPath != Other.ObjectPath;
	}

	bool operator>(const FFileData& Other) const
	{
		return  Other.ObjectPath.LexicalLess(ObjectPath);
	}

	bool operator<(const FFileData& Other) const
	{
		return ObjectPath.LexicalLess(Other.ObjectPath);
	}

	/** Checks to see if this AssetData refers to an asset or is NULL */
	bool IsValid() const
	{
		return ObjectPath != NAME_None;
	}

	/** Returns true if this is the primary asset in a package, true for maps and assets but false for secondary objects like class redirectors */
	bool IsUAsset() const
	{
		return FPackageName::GetLongPackageAssetName(PackageName.ToString()) == AssetName.ToString();
	}

	void Shrink()
	{
		ChunkIDs.Shrink();
		TagsAndValues.Shrink();
	}

	/** Returns the full name for the asset in the form: Class ObjectPath */
	FString GetFullName() const
	{
		FString FullName;
		GetFullName(FullName);
		return FullName;
	}

	/** Populates OutFullName with the full name for the asset in the form: Class ObjectPath */
	void GetFullName(FString& OutFullName) const
	{
		OutFullName.Reset();
		AssetClass.AppendString(OutFullName);
		OutFullName.AppendChar(' ');
		ObjectPath.AppendString(OutFullName);
	}

	/** Returns the name for the asset in the form: Class'ObjectPath' */
	FString GetExportTextName() const
	{
		FString ExportTextName;
		GetExportTextName(ExportTextName);
		return ExportTextName;
	}

	/** Populates OutExportTextName with the name for the asset in the form: Class'ObjectPath' */
	void GetExportTextName(FString& OutExportTextName) const
	{
		OutExportTextName.Reset();
		AssetClass.AppendString(OutExportTextName);
		OutExportTextName.AppendChar('\'');
		ObjectPath.AppendString(OutExportTextName);
		OutExportTextName.AppendChar('\'');
	}

	/** Returns true if the this asset is a redirector. */
	bool IsRedirector() const
	{
		if (AssetClass == UObjectRedirector::StaticClass()->GetFName())
		{
			return true;
		}

		return false;
	}

	/** Returns the class UClass if it is loaded. It is not possible to load the class if it is unloaded since we only have the short name. */
	UClass* GetClass() const
	{
		if (!IsValid())
		{
			// Dont even try to find the class if the objectpath isn't set
			return NULL;
		}

		UClass* FoundClass = FindObject<UClass>(ANY_PACKAGE, *AssetClass.ToString());

		if (!FoundClass)
		{
			// Look for class redirectors
			FName NewPath = FLinkerLoad::FindNewNameForClass(AssetClass, false);

			if (NewPath != NAME_None)
			{
				FoundClass = FindObject<UClass>(ANY_PACKAGE, *NewPath.ToString());
			}
		}
		return FoundClass;
	}

	/** Convert to a SoftObjectPath for loading */
	FSoftObjectPath ToSoftObjectPath() const
	{
		return FSoftObjectPath(ObjectPath.ToString());
	}

	UE_DEPRECATED(4.18, "ToStringReference was renamed to ToSoftObjectPath")
		FSoftObjectPath ToStringReference() const
	{
		return ToSoftObjectPath();
	}

	/** Gets primary asset id of this data */
	FPrimaryAssetId GetPrimaryAssetId() const
	{
		FName PrimaryAssetType, PrimaryAssetName;
		GetTagValueNameImpl(FPrimaryAssetId::PrimaryAssetTypeTag, PrimaryAssetType);
		GetTagValueNameImpl(FPrimaryAssetId::PrimaryAssetNameTag, PrimaryAssetName);

		if (PrimaryAssetType != NAME_None && PrimaryAssetName != NAME_None)
		{
			return FPrimaryAssetId(PrimaryAssetType, PrimaryAssetName);
		}

		return FPrimaryAssetId();
	}

	/** Returns the asset UObject if it is loaded or loads the asset if it is unloaded then returns the result */
	UObject* FastGetAsset(bool bLoad = false) const
	{
		if (!IsValid())
		{
			// Do not try to find the object if the objectpath is not set
			return NULL;
		}

		UPackage* FoundPackage = FindObjectFast<UPackage>(nullptr, PackageName);
		if (FoundPackage == NULL)
		{
			if (bLoad)
			{
				return LoadObject<UObject>(NULL, *ObjectPath.ToString());
			}
			else
			{
				return NULL;
			}
		}

		UObject* Asset = FindObjectFast<UObject>(FoundPackage, AssetName);
		if (Asset == NULL && bLoad)
		{
			return LoadObject<UObject>(NULL, *ObjectPath.ToString());
		}

		return Asset;
	}

	/** Returns the asset UObject if it is loaded or loads the asset if it is unloaded then returns the result */
	UObject* GetAsset() const
	{
		if (!IsValid())
		{
			// Dont even try to find the object if the objectpath isn't set
			return NULL;
		}

		UObject* Asset = FindObject<UObject>(NULL, *ObjectPath.ToString());
		if (Asset == NULL)
		{
			Asset = LoadObject<UObject>(NULL, *ObjectPath.ToString());
		}

		return Asset;
	}

	UPackage* GetPackage() const
	{
		if (PackageName == NAME_None)
		{
			return NULL;
		}

		UPackage* Package = FindPackage(NULL, *PackageName.ToString());
		if (Package)
		{
			Package->FullyLoad();
		}
		else
		{
			Package = LoadPackage(NULL, *PackageName.ToString(), LOAD_None);
		}

		return Package;
	}

	/** Try and get the value associated with the given tag as a type converted value */
	template <typename ValueType>
	bool GetTagValue(const FName InTagName, ValueType& OutTagValue) const;

	/** Try and get the value associated with the given tag as a type converted value, or an empty value if it doesn't exist */
	template <typename ValueType>
	ValueType GetTagValueRef(const FName InTagName) const;

	/** Returns true if the asset is loaded */
	bool IsAssetLoaded() const
	{
		return IsValid() && FindObjectSafe<UObject>(NULL, *ObjectPath.ToString()) != NULL;
	}

	/** Prints the details of the asset to the log */
	void PrintAssetData() const
	{
		UE_LOG(LogAssetData, Log, TEXT("    FFileData for %s"), *ObjectPath.ToString());
		UE_LOG(LogAssetData, Log, TEXT("    ============================="));
		UE_LOG(LogAssetData, Log, TEXT("        PackageName: %s"), *PackageName.ToString());
		UE_LOG(LogAssetData, Log, TEXT("        PackagePath: %s"), *PackagePath.ToString());
		UE_LOG(LogAssetData, Log, TEXT("        AssetName: %s"), *AssetName.ToString());
		UE_LOG(LogAssetData, Log, TEXT("        AssetClass: %s"), *AssetClass.ToString());
		UE_LOG(LogAssetData, Log, TEXT("        TagsAndValues: %d"), TagsAndValues.Num());

		for (const auto& TagValue : TagsAndValues)
		{
			UE_LOG(LogAssetData, Log, TEXT("            %s : %s"), *TagValue.Key.ToString(), *FString(TagValue.Value));
		}

		UE_LOG(LogAssetData, Log, TEXT("        ChunkIDs: %d"), ChunkIDs.Num());

		for (int32 Chunk : ChunkIDs)
		{
			UE_LOG(LogAssetData, Log, TEXT("                 %d"), Chunk);
		}

		UE_LOG(LogAssetData, Log, TEXT("        PackageFlags: %d"), PackageFlags);
	}

	/** Get the first FFileData of a particular class from an Array of FFileData */
	static FFileData GetFirstAssetDataOfClass(const TArray<FFileData>& Assets, const UClass* DesiredClass)
	{
		for (int32 AssetIdx = 0; AssetIdx < Assets.Num(); AssetIdx++)
		{
			const FFileData& Data = Assets[AssetIdx];
			UClass* AssetClass = Data.GetClass();
			if (AssetClass != NULL && AssetClass->IsChildOf(DesiredClass))
			{
				return Data;
			}
		}
		return FFileData();
	}

	/** Convenience template for finding first asset of a class */
	template <class T>
	static T* GetFirstAsset(const TArray<FFileData>& Assets)
	{
		UClass* DesiredClass = T::StaticClass();
		UObject* Asset = FFileData::GetFirstAssetDataOfClass(Assets, DesiredClass).GetAsset();
		check(Asset == NULL || Asset->IsA(DesiredClass));
		return (T*)Asset;
	}

	/**
	 * Serialize as part of the registry cache. This is not meant to be serialized as part of a package so  it does not handle versions normally
	 * To version this data change FAssetRegistryVersion
	 */
	void SerializeForCache(FArchive& Ar)
	{
		// Serialize out the asset info
		Ar << ObjectPath;
		Ar << PackagePath;
		Ar << AssetClass;

		// These are derived from ObjectPath, we manually serialize them because they get pooled
		Ar << PackageName;
		Ar << AssetName;

		Ar << TagsAndValues;
		Ar << ChunkIDs;
		Ar << PackageFlags;
	}

private:
	bool GetTagValueStringImpl(const FName InTagName, FString& OutTagValue) const
	{
		const FFileDataTagMapSharedView::FFindTagResult FoundValue = TagsAndValues.FindTag(InTagName);
		if (FoundValue.IsSet())
		{
			const FString& FoundString(FoundValue.GetValue());
			bool bIsHandled = false;
			if (FTextStringHelper::IsComplexText(*FoundString))
			{
				FText TmpText;
				if (FTextStringHelper::ReadFromBuffer(*FoundString, TmpText))
				{
					bIsHandled = true;
					OutTagValue = TmpText.ToString();
				}
			}

			if (!bIsHandled)
			{
				OutTagValue = FoundString;
			}

			return true;
		}

		return false;
	}
	bool GetTagValueTextImpl(const FName InTagName, FText& OutTagValue) const
	{
		const FFileDataTagMapSharedView::FFindTagResult FoundValue = TagsAndValues.FindTag(InTagName);
		if (FoundValue.IsSet())
		{
			const FString& FoundString(FoundValue.GetValue());
			if (!FTextStringHelper::ReadFromBuffer(*FoundString, OutTagValue))
			{
				OutTagValue = FText::FromString(FoundString);
			}
			return true;
		}
		return false;
	}

	bool GetTagValueNameImpl(const FName InTagName, FName& OutTagValue) const
	{
		FString StrValue;
		if (GetTagValueStringImpl(InTagName, StrValue))
		{
			OutTagValue = *StrValue;
			return true;
		}
		return false;
	}
};