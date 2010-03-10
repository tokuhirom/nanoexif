#include <stdint.h>
#include <stdio.h>

const char *nanoexif_tag_name(uint32_t n) {
    switch (n) {
    case 0x0001: return "InteropIndex";
    case 0x0002: return "InteropVersion";
    case 0x000B: return "ProcessingSoftware";
    case 0x00FE: return "SubfileType";
    case 0x00FF: return "OldSubfileType";
    case 0x0100: return "ImageWidth";
    case 0x0101: return "ImageHeight";
    case 0x0102: return "BitsPerSample";
    case 0x0103: return "Compression";
    case 0x0106: return "PhotometricInterpretation";
    case 0x0107: return "Thresholding";
    case 0x0108: return "CellWidth";
    case 0x0109: return "CellLength";
    case 0x010A: return "FillOrder";
    case 0x010D: return "DocumentName";
    case 0x010E: return "ImageDescription";
    case 0x010F: return "Make";
    case 0x0110: return "Model";
    case 0x0112: return "Orientation";
    case 0x0115: return "SamplesPerPixel";
    case 0x0116: return "RowsPerStrip";
    case 0x0118: return "MinSampleValue";
    case 0x0119: return "MaxSampleValue";
    case 0x011A: return "XResolution";
    case 0x011B: return "YResolution";
    case 0x011C: return "PlanarConfiguration";
    case 0x011D: return "PageName";
    case 0x011E: return "XPosition";
    case 0x011F: return "YPosition";
    case 0x0120: return "FreeOffsets";
    case 0x0121: return "FreeByteCounts";
    case 0x0122: return "GrayResponseUnit";
    case 0x0123: return "GrayResponseCurve";
    case 0x0124: return "T4Options";
    case 0x0125: return "T6Options";
    case 0x0128: return "ResolutionUnit";
    case 0x0129: return "PageNumber";
    case 0x012C: return "ColorResponseUnit";
    case 0x012D: return "TransferFunction";
    case 0x0131: return "Software";
    case 0x0132: return "ModifyDate";
    case 0x013B: return "Artist";
    case 0x013C: return "HostComputer";
    case 0x013D: return "Predictor";
    case 0x013E: return "WhitePoint";
    case 0x013F: return "PrimaryChromaticities";
    case 0x0140: return "ColorMap";
    case 0x0141: return "HalftoneHints";
    case 0x0142: return "TileWidth";
    case 0x0143: return "TileLength";
    case 0x0144: return "TileOffsets";
    case 0x0145: return "TileByteCounts";
    case 0x0146: return "BadFaxLines";
    case 0x0147: return "CleanFaxData";
    case 0x0148: return "ConsecutiveBadFaxLines";
    case 0x014C: return "InkSet";
    case 0x014D: return "InkNames";
    case 0x014E: return "NumberofInks";
    case 0x0150: return "DotRange";
    case 0x0151: return "TargetPrinter";
    case 0x0152: return "ExtraSamples";
    case 0x0153: return "SampleFormat";
    case 0x0154: return "SMinSampleValue";
    case 0x0155: return "SMaxSampleValue";
    case 0x0156: return "TransferRange";
    case 0x0157: return "ClipPath";
    case 0x0158: return "XClipPathUnits";
    case 0x0159: return "YClipPathUnits";
    case 0x015A: return "Indexed";
    case 0x015B: return "JPEGTables";
    case 0x015F: return "OPIProxy";
    case 0x0190: return "GlobalParametersIFD";
    case 0x0191: return "ProfileType";
    case 0x0192: return "FaxProfile";
    case 0x0193: return "CodingMethods";
    case 0x0194: return "VersionYear";
    case 0x0195: return "ModeNumber";
    case 0x01B1: return "Decode";
    case 0x01B2: return "DefaultImageColor";
    case 0x01B5: return "JPEGTables";
    case 0x0200: return "JPEGProc";
    case 0x0203: return "JPEGRestartInterval";
    case 0x0205: return "JPEGLosslessPredictors";
    case 0x0206: return "JPEGPointTransforms";
    case 0x0207: return "JPEGQTables";
    case 0x0208: return "JPEGDCTables";
    case 0x0209: return "JPEGACTables";
    case 0x0211: return "YCbCrCoefficients";
    case 0x0212: return "YCbCrSubSampling";
    case 0x0213: return "YCbCrPositioning";
    case 0x0214: return "ReferenceBlackWhite";
    case 0x022F: return "StripRowCounts";
    case 0x02BC: return "ApplicationNotes";
    case 0x1000: return "RelatedImageFileFormat";
    case 0x1001: return "RelatedImageWidth";
    case 0x1002: return "RelatedImageHeight";
    case 0x4746: return "Rating";
    case 0x4749: return "RatingPercent";
    case 0x800D: return "ImageID";
    case 0x80A4: return "WangAnnotation";
    case 0x80E3: return "Matteing";
    case 0x80E4: return "DataType";
    case 0x80E5: return "ImageDepth";
    case 0x80E6: return "TileDepth";
    case 0x827D: return "Model2";
    case 0x828D: return "CFARepeatPatternDim";
    case 0x828E: return "CFAPattern2";
    case 0x828F: return "BatteryLevel";
    case 0x8290: return "KodakIFD";
    case 0x8298: return "Copyright";
    case 0x829A: return "ExposureTime";
    case 0x829D: return "FNumber";
    case 0x82A5: return "MDFileTag";
    case 0x82A6: return "MDScalePixel";
    case 0x82A7: return "MDColorTable";
    case 0x82A8: return "MDLabName";
    case 0x82A9: return "MDSampleInfo";
    case 0x82AA: return "MDPrepDate";
    case 0x82AB: return "MDPrepTime";
    case 0x82AC: return "MDFileUnits";
    case 0x830E: return "PixelScale";
    case 0x83BB: return "IPTC-NAA";
    case 0x847E: return "IntergraphPacketData";
    case 0x847F: return "IntergraphFlagRegisters";
    case 0x8480: return "IntergraphMatrix";
    case 0x8482: return "ModelTiePoint";
    case 0x84E0: return "Site";
    case 0x84E1: return "ColorSequence";
    case 0x84E2: return "IT8Header";
    case 0x84E3: return "RasterPadding";
    case 0x84E4: return "BitsPerRunLength";
    case 0x84E5: return "BitsPerExtendedRunLength";
    case 0x84E6: return "ColorTable";
    case 0x84E7: return "ImageColorIndicator";
    case 0x84E8: return "BackgroundColorIndicator";
    case 0x84E9: return "ImageColorValue";
    case 0x84EA: return "BackgroundColorValue";
    case 0x84EB: return "PixelIntensityRange";
    case 0x84EC: return "TransparencyIndicator";
    case 0x84ED: return "ColorCharacterization";
    case 0x84EE: return "HCUsage";
    case 0x84EF: return "TrapIndicator";
    case 0x84F0: return "CMYKEquivalent";
    case 0x8546: return "SEMInfo";
    case 0x8568: return "AFCP_IPTC";
    case 0x85D8: return "ModelTransform";
    case 0x8602: return "WB_GRGBLevels";
    case 0x8606: return "LeafData";
    case 0x8649: return "PhotoshopSettings";
    case 0x8769: return "ExifOffset";
    case 0x8773: return "ICC_Profile";
    case 0x87AC: return "ImageLayer";
    case 0x87AF: return "GeoTiffDirectory";
    case 0x87B0: return "GeoTiffDoubleParams";
    case 0x87B1: return "GeoTiffAsciiParams";
    case 0x8822: return "ExposureProgram";
    case 0x8824: return "SpectralSensitivity";
    case 0x8825: return "GPSInfo";
    case 0x8827: return "ISO";
    case 0x8828: return "Opto-ElectricConvFactor";
    case 0x8829: return "Interlace";
    case 0x882A: return "TimeZoneOffset";
    case 0x882B: return "SelfTimerMode";
    case 0x885C: return "FaxRecvParams";
    case 0x885D: return "FaxSubAddress";
    case 0x885E: return "FaxRecvTime";
    case 0x888A: return "LeafSubIFD";
    case 0x9000: return "ExifVersion";
    case 0x9003: return "DateTimeOriginal";
    case 0x9004: return "CreateDate";
    case 0x9101: return "ComponentsConfiguration";
    case 0x9102: return "CompressedBitsPerPixel";
    case 0x9201: return "ShutterSpeedValue";
    case 0x9202: return "ApertureValue";
    case 0x9203: return "BrightnessValue";
    case 0x9204: return "ExposureCompensation";
    case 0x9205: return "MaxApertureValue";
    case 0x9206: return "SubjectDistance";
    case 0x9207: return "MeteringMode";
    case 0x9208: return "LightSource";
    case 0x9209: return "Flash";
    case 0x920A: return "FocalLength";
    case 0x920B: return "FlashEnergy";
    case 0x920C: return "SpatialFrequencyResponse";
    case 0x920D: return "Noise";
    case 0x920E: return "FocalPlaneXResolution";
    case 0x920F: return "FocalPlaneYResolution";
    case 0x9210: return "FocalPlaneResolutionUnit";
    case 0x9211: return "ImageNumber";
    case 0x9212: return "SecurityClassification";
    case 0x9213: return "ImageHistory";
    case 0x9214: return "SubjectArea";
    case 0x9215: return "ExposureIndex";
    case 0x9216: return "TIFF-EPStandardID";
    case 0x9217: return "SensingMethod";
    case 0x923F: return "StoNits";
    case 0x9286: return "UserComment";
    case 0x9290: return "SubSecTime";
    case 0x9291: return "SubSecTimeOriginal";
    case 0x9292: return "SubSecTimeDigitized";
    case 0x932F: return "MSDocumentText";
    case 0x9330: return "MSPropertySetStorage";
    case 0x9331: return "MSDocumentTextPosition";
    case 0x935C: return "ImageSourceData";
    case 0x9C9B: return "XPTitle";
    case 0x9C9C: return "XPComment";
    case 0x9C9D: return "XPAuthor";
    case 0x9C9E: return "XPKeywords";
    case 0x9C9F: return "XPSubject";
    case 0xA000: return "FlashpixVersion";
    case 0xA001: return "ColorSpace";
    case 0xA002: return "ExifImageWidth";
    case 0xA003: return "ExifImageHeight";
    case 0xA004: return "RelatedSoundFile";
    case 0xA005: return "InteropOffset";
    case 0xA20B: return "FlashEnergy";
    case 0xA20C: return "SpatialFrequencyResponse";
    case 0xA20D: return "Noise";
    case 0xA20E: return "FocalPlaneXResolution";
    case 0xA20F: return "FocalPlaneYResolution";
    case 0xA210: return "FocalPlaneResolutionUnit";
    case 0xA211: return "ImageNumber";
    case 0xA212: return "SecurityClassification";
    case 0xA213: return "ImageHistory";
    case 0xA214: return "SubjectLocation";
    case 0xA215: return "ExposureIndex";
    case 0xA216: return "TIFF-EPStandardID";
    case 0xA217: return "SensingMethod";
    case 0xA300: return "FileSource";
    case 0xA301: return "SceneType";
    case 0xA302: return "CFAPattern";
    case 0xA401: return "CustomRendered";
    case 0xA402: return "ExposureMode";
    case 0xA403: return "WhiteBalance";
    case 0xA404: return "DigitalZoomRatio";
    case 0xA405: return "FocalLengthIn35mmFormat";
    case 0xA406: return "SceneCaptureType";
    case 0xA407: return "GainControl";
    case 0xA408: return "Contrast";
    case 0xA409: return "Saturation";
    case 0xA40A: return "Sharpness";
    case 0xA40B: return "DeviceSettingDescription";
    case 0xA40C: return "SubjectDistanceRange";
    case 0xA420: return "ImageUniqueID";
    case 0xA480: return "GDALMetadata";
    case 0xA481: return "GDALNoData";
    case 0xA500: return "Gamma";
    case 0xAFC0: return "ExpandSoftware";
    case 0xAFC1: return "ExpandLens";
    case 0xAFC2: return "ExpandFilm";
    case 0xAFC3: return "ExpandFilterLens";
    case 0xAFC4: return "ExpandScanner";
    case 0xAFC5: return "ExpandFlashLamp";
    case 0xBC01: return "PixelFormat";
    case 0xBC02: return "Transformation";
    case 0xBC03: return "Uncompressed";
    case 0xBC04: return "ImageType";
    case 0xBC80: return "ImageWidth";
    case 0xBC81: return "ImageHeight";
    case 0xBC82: return "WidthResolution";
    case 0xBC83: return "HeightResolution";
    case 0xBCC0: return "ImageOffset";
    case 0xBCC1: return "ImageByteCount";
    case 0xBCC2: return "AlphaOffset";
    case 0xBCC3: return "AlphaByteCount";
    case 0xBCC4: return "ImageDataDiscard";
    case 0xBCC5: return "AlphaDataDiscard";
    case 0xC427: return "OceScanjobDesc";
    case 0xC428: return "OceApplicationSelector";
    case 0xC429: return "OceIDNumber";
    case 0xC42A: return "OceImageLogic";
    case 0xC44F: return "Annotations";
    case 0xC4A5: return "PrintIM";
    case 0xC612: return "DNGVersion";
    case 0xC613: return "DNGBackwardVersion";
    case 0xC614: return "UniqueCameraModel";
    case 0xC615: return "LocalizedCameraModel";
    case 0xC616: return "CFAPlaneColor";
    case 0xC617: return "CFALayout";
    case 0xC618: return "LinearizationTable";
    case 0xC619: return "BlackLevelRepeatDim";
    case 0xC61A: return "BlackLevel";
    case 0xC61B: return "BlackLevelDeltaH";
    case 0xC61C: return "BlackLevelDeltaV";
    case 0xC61D: return "WhiteLevel";
    case 0xC61E: return "DefaultScale";
    case 0xC61F: return "DefaultCropOrigin";
    case 0xC620: return "DefaultCropSize";
    case 0xC621: return "ColorMatrix1";
    case 0xC622: return "ColorMatrix2";
    case 0xC623: return "CameraCalibration1";
    case 0xC624: return "CameraCalibration2";
    case 0xC625: return "ReductionMatrix1";
    case 0xC626: return "ReductionMatrix2";
    case 0xC627: return "AnalogBalance";
    case 0xC628: return "AsShotNeutral";
    case 0xC629: return "AsShotWhiteXY";
    case 0xC62A: return "BaselineExposure";
    case 0xC62B: return "BaselineNoise";
    case 0xC62C: return "BaselineSharpness";
    case 0xC62D: return "BayerGreenSplit";
    case 0xC62E: return "LinearResponseLimit";
    case 0xC62F: return "CameraSerialNumber";
    case 0xC630: return "DNGLensInfo";
    case 0xC631: return "ChromaBlurRadius";
    case 0xC632: return "AntiAliasStrength";
    case 0xC633: return "ShadowScale";
    case 0xC635: return "MakerNoteSafety";
    case 0xC640: return "RawImageSegmentation";
    case 0xC65A: return "CalibrationIlluminant1";
    case 0xC65B: return "CalibrationIlluminant2";
    case 0xC65C: return "BestQualityScale";
    case 0xC65D: return "RawDataUniqueID";
    case 0xC660: return "AliasLayerMetadata";
    case 0xC68B: return "OriginalRawFileName";
    case 0xC68C: return "OriginalRawFileData";
    case 0xC68D: return "ActiveArea";
    case 0xC68E: return "MaskedAreas";
    case 0xC68F: return "AsShotICCProfile";
    case 0xC690: return "AsShotPreProfileMatrix";
    case 0xC691: return "CurrentICCProfile";
    case 0xC692: return "CurrentPreProfileMatrix";
    case 0xC6BF: return "ColorimetricReference";
    case 0xC6D2: return "PanasonicTitle";
    case 0xC6D3: return "PanasonicTitle2";
    case 0xC6F3: return "CameraCalibrationSig";
    case 0xC6F4: return "ProfileCalibrationSig";
    case 0xC6F5: return "ProfileIFD";
    case 0xC6F6: return "AsShotProfileName";
    case 0xC6F7: return "NoiseReductionApplied";
    case 0xC6F8: return "ProfileName";
    case 0xC6F9: return "ProfileHueSatMapDims";
    case 0xC6FA: return "ProfileHueSatMapData1";
    case 0xC6FB: return "ProfileHueSatMapData2";
    case 0xC6FC: return "ProfileToneCurve";
    case 0xC6FD: return "ProfileEmbedPolicy";
    case 0xC6FE: return "ProfileCopyright";
    case 0xC714: return "ForwardMatrix1";
    case 0xC715: return "ForwardMatrix2";
    case 0xC716: return "PreviewApplicationName";
    case 0xC717: return "PreviewApplicationVersion";
    case 0xC718: return "PreviewSettingsName";
    case 0xC719: return "PreviewSettingsDigest";
    case 0xC71A: return "PreviewColorSpace";
    case 0xC71B: return "PreviewDateTime";
    case 0xC71C: return "RawImageDigest";
    case 0xC71D: return "OriginalRawFileDigest";
    case 0xC71E: return "SubTileBlockSize";
    case 0xC71F: return "RowInterleaveFactor";
    case 0xC725: return "ProfileLookTableDims";
    case 0xC726: return "ProfileLookTableData";
    case 0xC740: return "OpcodeList1";
    case 0xC741: return "OpcodeList2";
    case 0xC74E: return "OpcodeList3";
    case 0xC761: return "NoiseProfile";
    case 0xEA1C: return "Padding";
    case 0xEA1D: return "OffsetSchema";
    case 0xFE00: return "KDC_IFD";
    }
    return NULL;
}