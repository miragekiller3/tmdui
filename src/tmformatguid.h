/*
 *  @file
 *  @brief  debug function for GUID
 *
 *  @author miragekiller <3916345933@qq.com>
 *  @date   2018.12.22
 *
 *  Copyright (C) 2018 miragekiller
 */

#pragma once

#ifndef TMFORMATGUID_H
#define TMFORMATGUID_H

#include "tmstring.h"
#include "tmcom.h"
#include "tmlist.h"
#include "tmgdi.h"


struct _CGUIDMap_for_format : public TM::CMap<GUID, TMCStr, TM::LessGUID>
{
    inline void Insert(LPCSTR guid, TMCStr face)
    {
        UUID uid;
        UuidFromStringA((unsigned char*)guid, &uid);
        insert2(uid, face);
    }

    _CGUIDMap_for_format()
    {
#define GUID_PAIR(x,y) typedef struct TM_LINK_OF(y,__LINE__) TM_LINK_OF(y,a); Insert(#x, TM_T(#y))
        GUID_PAIR(00000000-0000-0000-C000-000000000046, IUnknown);
        GUID_PAIR(00020400-0000-0000-C000-000000000046, IDispatch);
        GUID_PAIR(00000001-0000-0000-C000-000000000046, IClassFactory);
        GUID_PAIR(00000003-0000-0000-C000-000000000046, IMarshal);
        GUID_PAIR(00000002-0000-0000-C000-000000000046, IMalloc);
        GUID_PAIR(0000010c-0000-0000-C000-000000000046, IPersist);
        GUID_PAIR(0000000f-0000-0000-C000-000000000046, IMoniker);
        GUID_PAIR(0000000c-0000-0000-C000-000000000046, IStream);
        GUID_PAIR(0000000b-0000-0000-C000-000000000046, IStorage);
        GUID_PAIR(0000010b-0000-0000-C000-000000000046, IPersistFile);
        GUID_PAIR(0000010a-0000-0000-C000-000000000046, IPersistStorage);
        GUID_PAIR(0000010e-0000-0000-C000-000000000046, IDataObject);
        GUID_PAIR(0000001B-0000-0000-C000-000000000046, IIdentityUnmarshall);
        GUID_PAIR(00000018-0000-0000-C000-000000000046, IStdMarshallInfo);
        GUID_PAIR(000e0000-0000-0000-c000-000000000046, AsyncIUnknown);
        GUID_PAIR(00000017-0000-0000-c000-000000000046, IActivationFilter);
        GUID_PAIR(00000018-0000-0000-c000-000000000046, IStdMarshalInfo);
        GUID_PAIR(00000019-0000-0000-c000-000000000046, IExternalConnection);
        GUID_PAIR(00000020-0000-0000-c000-000000000046, IMultiQI);
        GUID_PAIR(000e0020-0000-0000-c000-000000000046, AsyncIMultiQI);
        GUID_PAIR(00000021-0000-0000-c000-000000000046, IInternalUnknown);
        GUID_PAIR(00000100-0000-0000-c000-000000000046, IEnumUnknown);
        GUID_PAIR(00000101-0000-0000-c000-000000000046, IEnumString);
        GUID_PAIR(00000144-0000-0000-c000-000000000046, IRpcOptions);
        GUID_PAIR(00000022-0000-0000-c000-000000000046, ISurrogate);
        GUID_PAIR(00000146-0000-0000-c000-000000000046, IGlobalInterfaceTable);
        GUID_PAIR(00000030-0000-0000-c000-000000000046, ISynchronize);
        GUID_PAIR(00000031-0000-0000-c000-000000000046, ISynchronizeHandle);
        GUID_PAIR(00000032-0000-0000-c000-000000000046, ISynchronizeEvent);
        GUID_PAIR(00000033-0000-0000-c000-000000000046, ISynchronizeContainer);
        GUID_PAIR(00000025-0000-0000-c000-000000000046, ISynchronizeMutex);
        GUID_PAIR(00000029-0000-0000-c000-000000000046, ICancelMethodCalls);
        GUID_PAIR(00000149-0000-0000-c000-000000000046, IRpcHelper);
        GUID_PAIR(00000147-0000-0000-c000-000000000046, IAddrTrackingControl);
        GUID_PAIR(00000148-0000-0000-c000-000000000046, IAddrExclusionControl);
        GUID_PAIR(000001c1-0000-0000-c000-000000000046, IEnumContextProps);
        GUID_PAIR(000001c0-0000-0000-c000-000000000046, IContext);
        GUID_PAIR(000001c6-0000-0000-c000-000000000046, IObjContext);
        GUID_PAIR(00000040-0000-0000-c000-000000000046, IFastRundown);
        GUID_PAIR(00000102-0000-0000-c000-000000000046, IEnumMoniker);
        GUID_PAIR(00000126-0000-0000-c000-000000000046, IRunnableObject);
        GUID_PAIR(00000010-0000-0000-c000-000000000046, IRunningObjectTable);
        GUID_PAIR(00000109-0000-0000-c000-000000000046, IPersistStream);
        GUID_PAIR(00000103-0000-0000-c000-000000000046, IEnumFORMATETC);
        GUID_PAIR(00000105-0000-0000-c000-000000000046, IEnumSTATDATA);
        GUID_PAIR(00000012-0000-0000-c000-000000000046, IRootStorage);
        GUID_PAIR(00000150-0000-0000-c000-000000000046, AsyncIAdviseSink);
        GUID_PAIR(00000125-0000-0000-c000-000000000046, IAdviseSink2);
        GUID_PAIR(00000151-0000-0000-c000-000000000046, AsyncIAdviseSink2);
        GUID_PAIR(00000110-0000-0000-c000-000000000046, IDataAdviseHolder);
        GUID_PAIR(00000016-0000-0000-c000-000000000046, IMessageFilter);
        GUID_PAIR(00000140-0000-0000-c000-000000000046, IClassActivator);
        GUID_PAIR(00000026-0000-0000-c000-000000000046, IUrlMon);
        GUID_PAIR(00000145-0000-0000-c000-000000000046, IForegroundTransfer);
        GUID_PAIR(000001d5-0000-0000-c000-000000000046, IProcessLock);
        GUID_PAIR(000001d4-0000-0000-c000-000000000046, ISurrogateService);
        GUID_PAIR(00000034-0000-0000-c000-000000000046, IInitializeSpy);
        GUID_PAIR(00020410-0000-0000-c000-000000000046, ITypeChangeEvents);
        GUID_PAIR(00000138-0000-0000-c000-000000000046, IPropertyStorage);
        GUID_PAIR(00000139-0000-0000-c000-000000000046, IEnumSTATPROPSTG);
        GUID_PAIR(00000111-0000-0000-c000-000000000046, IOleAdviseHolder);
        GUID_PAIR(00000128-0000-0000-c000-000000000046, IOleCache2);
        GUID_PAIR(00000129-0000-0000-c000-000000000046, IOleCacheControl);
        GUID_PAIR(00000118-0000-0000-c000-000000000046, IOleClientSite);
        GUID_PAIR(00000112-0000-0000-c000-000000000046, IOleObject);
        GUID_PAIR(00000114-0000-0000-c000-000000000046, IOleWindow);
        GUID_PAIR(00000115-0000-0000-c000-000000000046, IOleInPlaceUIWindow);
        GUID_PAIR(00000117-0000-0000-c000-000000000046, IOleInPlaceActiveObject);
        GUID_PAIR(00000116-0000-0000-c000-000000000046, IOleInPlaceFrame);
        GUID_PAIR(00000113-0000-0000-c000-000000000046, IOleInPlaceObject);
        GUID_PAIR(00000119-0000-0000-c000-000000000046, IOleInPlaceSite);
        GUID_PAIR(00000127-0000-0000-c000-000000000046, IViewObject2);
        GUID_PAIR(00000121-0000-0000-c000-000000000046, IDropSource);
        GUID_PAIR(00000122-0000-0000-c000-000000000046, IDropTarget);
        GUID_PAIR(00000104-0000-0000-c000-000000000046, IEnumOLEVERB);
        GUID_PAIR(0002df05-0000-0000-c000-000000000046, IWebBrowserApp);
        GUID_PAIR(00000037-0000-0000-c000-000000000046, IWeakReference);
        GUID_PAIR(00000038-0000-0000-c000-000000000046, IWeakReferenceSource);
        GUID_PAIR(0002e000-0000-0000-c000-000000000046, IEnumGUID);
        GUID_PAIR(0002e011-0000-0000-c000-000000000046, IEnumCATEGORYINFO);
        GUID_PAIR(0002e012-0000-0000-c000-000000000046, ICatRegister);
        GUID_PAIR(0002e013-0000-0000-c000-000000000046, ICatInformation);
        GUID_PAIR(000214e4-0000-0000-c000-000000000046, IContextMenu);
        GUID_PAIR(000214f4-0000-0000-c000-000000000046, IContextMenu2);
        GUID_PAIR(000214f2-0000-0000-c000-000000000046, IEnumIDList);
        GUID_PAIR(000214e6-0000-0000-c000-000000000046, IShellFolder);
        GUID_PAIR(000214e3-0000-0000-c000-000000000046, IShellView);
        GUID_PAIR(000214f1-0000-0000-c000-000000000046, ICommDlgBrowser);
        GUID_PAIR(000214e5-0000-0000-c000-000000000046, IShellIcon);
        GUID_PAIR(000214e2-0000-0000-c000-000000000046, IShellBrowser);
        GUID_PAIR(000214f9-0000-0000-c000-000000000046, IShellLinkW);
        GUID_PAIR(000214e8-0000-0000-c000-000000000046, IShellExtInit);
        GUID_PAIR(000214e9-0000-0000-c000-000000000046, IShellPropSheetExt);
        GUID_PAIR(000c0600-0000-0000-c000-000000000046, IOleComponent);
        GUID_PAIR(000c0601-0000-0000-c000-000000000046, IOleComponentManager);
        GUID_PAIR(00000035-0000-0000-c000-000000000046, IActivationFactory);
        GUID_PAIR(00000036-0000-0000-c000-000000000046, IAsyncInfo);
        GUID_PAIR(1c733a30-2a1c-11ce-ade5-00aa0044773d, ICallFactory);

        // mozilla
        GUID_PAIR(a5d8e1f3-3571-4d8f-9521-07ed28fb072e, IAccessibleContentDocument);

        // .net
        GUID_PAIR(805D7A98-D4AF-3F0F-967F-E5CF45312D2C, IDisposable);
        GUID_PAIR(65074f7f-63c0-304e-af0a-d51741cb4a8d, _Object);
        GUID_PAIR(4c1e39e1-e3e3-4296-aa86-ec938d896e92, IMarshalOptions);
        GUID_PAIR(9bc79c93-2289-4bb5-abf4-3287fd9cae39, IPimcContext);

        // window's Private QI'ing to return the base IUnknown. Does not work across apartments.
        GUID_PAIR(33f139ee-e509-47f7-bf39-837644f74576, IIS_AccWrapBase_GetIUnknown);

        // Accessible
        GUID_PAIR(618736e0-3c3d-11cf-810c-00aa00389b71, IAccessible);
        GUID_PAIR(7852b78d-1cfd-41c1-a615-9c0c85960b5f, IAccIdentity);
        GUID_PAIR(76c0dbbb-15e0-4e7b-b61b-20eeea2001e0, IAccPropServer);
        GUID_PAIR(6e26e776-04f0-495d-80e4-3330352e3169, IAccPropServices);
        GUID_PAIR(d6dd68d1-86fd-4332-8666-9abedea2d24c, IRawElementProviderSimple);
        GUID_PAIR(A0A839A9-8DA1-4A82-806A-8E0D44E79F56, IRawElementProviderSimple2);
        GUID_PAIR(f8b80ada-2c44-48d0-89be-5ff23c9cd875, IAccessibleEx);
        GUID_PAIR(620ce2a5-ab8f-40a9-86cb-de3c75599b58, IRawElementProviderFragmentRoot);
        GUID_PAIR(f7063da8-8359-439c-9297-bbc5299a7d87, IRawElementProviderFragment);
        GUID_PAIR(a407b27b-0f6d-4427-9292-473c7bf93258, IRawElementProviderAdviseEvents);
        GUID_PAIR(1d5df27c-8947-4425-b8d9-79787bb460b8, IRawElementProviderHwndOverride);
        GUID_PAIR(4fd82b78-a43e-46ac-9803-0a6969c7c183, IProxyProviderWinEventSink);
        GUID_PAIR(89592ad4-f4e0-43d5-a3b6-bad7e111b435, IProxyProviderWinEventHandler);
        GUID_PAIR(0a2a93cc-bfad-42ac-9b2e-0991fb0d3ea0, IRawElementProviderWindowlessSite);
        GUID_PAIR(33AC331B-943E-4020-B295-DB37784974A3, IAccessibleHostingElementProviders);
        GUID_PAIR(24BE0B07-D37D-487A-98CF-A13ED465E9B3, IRawElementProviderHostingAccessibles);
        GUID_PAIR(159bc72c-4ad3-485e-9637-d7052edf0146, IDockProvider);
        GUID_PAIR(d847d3a5-cab0-4a98-8c32-ecb45c59ad24, IExpandCollapseProvider);
        GUID_PAIR(b17d6187-0907-464b-a168-0ef17a1572b1, IGridProvider);
        GUID_PAIR(d02541f1-fb81-4d64-ae32-f520f8a6dbd1, IGridItemProvider);
        GUID_PAIR(54fcb24b-e18e-47a2-b4d3-eccbe77599a2, IInvokeProvider);
        GUID_PAIR(6278cab1-b556-4a1a-b4e0-418acc523201, IMultipleViewProvider);
        GUID_PAIR(36dc7aef-33e6-4691-afe1-2be7274b3d33, IRangeValueProvider);
        GUID_PAIR(2360c714-4bf1-4b26-ba65-9b21316127eb, IScrollItemProvider);
        GUID_PAIR(fb8b03af-3bdf-48d4-bd36-1a65793be168, ISelectionProvider);
        GUID_PAIR(b38b8077-1fc3-42a5-8cae-d40c2215055a, IScrollProvider);
        GUID_PAIR(2acad808-b2d4-452d-a407-91ff1ad167b2, ISelectionItemProvider);
        GUID_PAIR(29db1a06-02ce-4cf7-9b42-565d4fab20ee, ISynchronizedInputProvider);
        GUID_PAIR(9c860395-97b3-490a-b52a-858cc22af166, ITableProvider);
        GUID_PAIR(b9734fa6-771f-4d78-9c90-2517999349cd, ITableItemProvider);
        GUID_PAIR(56d00bd0-c4f4-433c-a836-1a52a57e0892, IToggleProvider);
        GUID_PAIR(6829ddc4-4f91-4ffa-b86f-bd3e2987cb4c, ITransformProvider);
        GUID_PAIR(c7935180-6fb3-4201-b174-7df73adbf64a, IValueProvider);
        GUID_PAIR(987df77b-db06-4d77-8f8a-86a9c3bb90b9, IWindowProvider);
        GUID_PAIR(e44c3566-915d-4070-99c6-047bff5a08f5, ILegacyIAccessibleProvider);
        GUID_PAIR(e747770b-39ce-4382-ab30-d8fb3f336f24, IItemContainerProvider);
        GUID_PAIR(cb98b665-2d35-4fac-ad35-f3c60d0c0b8b, IVirtualizedItemProvider);
        GUID_PAIR(3ad86ebd-f5ef-483d-bb18-b1042a475d64, IObjectModelProvider);
        GUID_PAIR(f95c7e80-bd63-4601-9782-445ebff011fc, IAnnotationProvider);
        GUID_PAIR(19b6b649-f5d7-4a6d-bdcb-129252be588a, IStylesProvider);
        GUID_PAIR(6f6b5d35-5525-4f80-b758-85473832ffc7, ISpreadsheetProvider);
        GUID_PAIR(eaed4660-7b3d-4879-a2e6-365ce603f3d0, ISpreadsheetItemProvider);
        GUID_PAIR(4758742f-7ac2-460c-bc48-09fc09308a93, ITransformProvider2);
        GUID_PAIR(6aa7bbbb-7ff9-497d-904f-d20b897929d8, IDragProvider);
        GUID_PAIR(bae82bfd-358a-481c-85a0-d8b4d90a5d61, IDropTargetProvider);
        GUID_PAIR(5347ad7b-c355-46f8-aff5-909033582f63, ITextRangeProvider);
        GUID_PAIR(3589c92c-63f3-4367-99bb-ada653b77cf2, ITextProvider);
        GUID_PAIR(0dc5e6ed-3e16-4bf1-8f9a-a979878bc195, ITextProvider2);
        GUID_PAIR(EA3605B4-3A05-400E-B5F9-4E91B40F6176, ITextEditProvider);
        GUID_PAIR(9BBCE42C-1921-4F18-89CA-DBA1910A0386, ITextRangeProvider2);
        GUID_PAIR(4c2de2b9-c88f-4f88-a111-f1d336b7d1a9, ITextChildProvider);
        GUID_PAIR(c03a7fe4-9431-409f-bed8-ae7c2299bc8d, IUIAutomationPatternInstance);
        GUID_PAIR(d97022f3-a947-465e-8b2a-ac4315fa54e8, IUIAutomationPatternHandler);
        GUID_PAIR(8609c4ec-4a1a-4d88-a357-5a66e060e1cf, IUIAutomationRegistrar);

        // IAccessible2
        GUID_PAIR(E89F726E-C4F4-4c19-BB19-B647D7FA8478, IAccessible2);
        GUID_PAIR(6C9430E9-299D-4E6F-BD01-A82A1E88D3FF, IAccessible2_2);
        GUID_PAIR(B70D9F59-3B5A-4dba-AB9E-22012F607DF5, IAccessibleAction);
        GUID_PAIR(D49DED83-5B25-43F4-9B95-93B44595979E, IAccessibleApplication);
        GUID_PAIR(1546D4B0-4C98-4bda-89AE-9A64748BDDE4, IAccessibleComponent);
        GUID_PAIR(C48C7FCF-4AB5-4056-AFA6-902D6E1D1149, IAccessibleDocument);
        GUID_PAIR(A59AA09A-7011-4b65-939D-32B1FB5547E3, IAccessibleEditableText);
        GUID_PAIR(01C20F2B-3DD2-400f-949F-AD00BDAB1D41, IAccessibleHyperlink);
        GUID_PAIR(6B4F8BBF-F1F2-418a-B35E-A195BC4103B9, IAccessibleHypertext);
        GUID_PAIR(CF64D89F-8287-4B44-8501-A827453A6077, IAccessibleHypertext2);
        GUID_PAIR(FE5ABB3D-615E-4f7b-909F-5F0EDA9E8DDE, IAccessibleImage);
        GUID_PAIR(7CDF86EE-C3DA-496a-BDA4-281B336E1FDC, IAccessibleRelation);
        GUID_PAIR(35AD8070-C20C-4fb4-B094-F4F7275DD469, IAccessibleTable);
        GUID_PAIR(6167f295-06f0-4cdd-a1fa-02e25153d869, IAccessibleTable2);
        GUID_PAIR(594116B1-C99F-4847-AD06-0A7A86ECE645, IAccessibleTableCell);
        GUID_PAIR(24FD2FFB-3AAD-4a08-8335-A3AD89C0FB4B, IAccessibleText);
        GUID_PAIR(9690A9CC-5C80-4DF5-852E-2D5AE4189A54, IAccessibleText2);
        GUID_PAIR(35855B5B-C566-4fd0-A7B1-E65465600394, IAccessibleValue);

        // html
        GUID_PAIR(3050f1ff-98b5-11cf-bb82-00aa00bdce0b, IHTMLElement);
        GUID_PAIR(3050f434-98b5-11cf-bb82-00aa00bdce0b, IHTMLElement2);

        GUID_PAIR(0000011B-0000-0000-C000-000000000046, IOleContainer);
        GUID_PAIR(94EA2B94-E9CC-49E0-C0FF-EE64CA8F5B90, IAgileObject);
        GUID_PAIR(c7a98e66-1010-492c-a1c8-c809e1f75905, IInternetProtocolEx);
        GUID_PAIR(4e9f1a77-4587-4235-81c4-e6d24545a656, IRequestCallbackCancel);
        GUID_PAIR(c5bc37d6-75c7-46a1-a132-81b5f723c20f, IMFMediaStream2);
        GUID_PAIR(5746e5c4-5b97-424c-b620-2822915734dd, ILanguageExceptionErrorInfo2);
        GUID_PAIR(205bebf8-dd93-452a-95a6-32b566b35828, IFsrmFileScreenTemplate);
        GUID_PAIR(9c243e11-3261-4bcd-b922-84a873d460ae, IProviderQueryConstraintCollection);
        GUID_PAIR(027947e1-d731-11ce-a357-000000000001, IEnumWbemClassObject);
        GUID_PAIR(548793c0-9e74-11cf-9655-00a0c9034923, ITargetEmbedding);
        GUID_PAIR(dfbb5136-9259-4895-b4a7-c1934429919a, IInternetExplorerManager2);
        GUID_PAIR(a3ccedf3-2de2-11d0-86f4-00a0c913f750, IImageDecodeFilter);
        GUID_PAIR(d934be07-7b84-4208-9136-83c20994e905, IInkRecognizerGuide);
        GUID_PAIR(75cf2c57-9195-4931-8332-f0b409e916af, IInputPaneInterop);
        GUID_PAIR(459ecc43-bcf5-11dc-a8a8-001321f1405f, IMbnSubscriberInformation);
        GUID_PAIR(df0daef4-a289-11d1-8697-006008b0e5d2, IMcastScope);
        GUID_PAIR(df0daf09-a289-11d1-8697-006008b0e5d2, IEnumMcastScope);
        GUID_PAIR(df0daef1-a289-11d1-8697-006008b0e5d2, IMcastAddressAllocation);
        GUID_PAIR(7a5e5354-b114-4c72-b991-3131d75032ea, IMFMediaEngineSrcElements);
        GUID_PAIR(3f96ee40-ad81-4096-8470-59a4b770f89a, IMFContentDecryptionModuleSessionCallbacks);
        GUID_PAIR(bab5f473-b423-477b-85f5-b5a332a04153, ISpatialAudioObjectRenderStream);
        GUID_PAIR(951ed750-d080-11d0-b197-000000000000, IMenuButton);
        GUID_PAIR(cc593830-b926-11d1-8063-0000f875a9ce, IDisplayHelp);
        GUID_PAIR(ce6db728-1357-46b2-a923-79f995f448fc, IPrintPageRangeOptions);
        GUID_PAIR(eb9b1606-9a36-4b59-8617-b217849262e1, IPrintTaskOptions2);
        GUID_PAIR(05f32de4-e384-51b3-b934-f0d3a0f78606, IHolographicFrameRenderingReport);
        GUID_PAIR(53730a09-f968-4692-a177-c074597186db, IPrintTaskOptionDetails2);
        GUID_PAIR(0a7d3da3-65d0-4086-80d6-0610d760207d, IInstalledClassicAppInfo);
        GUID_PAIR(e7e26076-a044-48e2-a512-3094d574c7cc, IAppRecordingManager);
        GUID_PAIR(5a14bc41-5b76-3140-b680-8825562683ac, IOcrEngine);
        GUID_PAIR(efab0782-f6e2-4675-a045-d8e320c24808, IMessageReceivedHandler);
        GUID_PAIR(efab0782-f6e1-4675-a045-d8e320c24808, IProximityMessage);
        GUID_PAIR(85047792-f634-41e3-96a4-5164e902c740, IOnlineIdSystemAuthenticatorStatics);
        GUID_PAIR(54dfd981-1968-4ca3-b958-e595d16505eb, IEasClientDeviceInformation);
        GUID_PAIR(6ce5e5f9-a0c9-4b2c-96a6-a171c630038d, IStoreRequestHelperStatics);
        GUID_PAIR(1664a082-0f38-5c99-83e4-48995970861c, IAppApplicabilityStatics);
        GUID_PAIR(1ee6e459-1705-4a9a-b599-a0c3d6921973, IWebAccountCommandInvokedHandler);
        GUID_PAIR(5d8ffc70-c618-4083-8251-9962593114aa, ISceneBoundingBox);
        GUID_PAIR(29138686-1963-49d8-9589-e14384c769e3, IInkAnalyzerFactory);
        GUID_PAIR(91dc62f8-42b3-47f3-8476-c55124a7c4c6, IPolyline);
        GUID_PAIR(5ed863d4-5535-48ac-98d4-c190995080b3, ISyndicationLinkFactory);
        GUID_PAIR(33bc7430-eec0-11d2-8201-00a0c9d74842, IDvdControl2);
        GUID_PAIR(34151510-eec0-11d2-8201-00a0c9d74842, IDvdInfo2);
        GUID_PAIR(4e1bd148-55a0-4480-a3d1-15544710637c, IAppxManifestReader);
        GUID_PAIR(34deaca4-d3c0-4e3e-b312-e42625e3807e, IAppxManifestReader6);
        GUID_PAIR(36537f36-27a4-4788-88c0-733819575017, IAppxManifestTargetDeviceFamiliesEnumerator);
        GUID_PAIR(0ded49d5-a8b7-4d5d-97a1-12b0c195874d, IBDA_PinControl);
        GUID_PAIR(cd51f1e0-7be9-4123-8482-a2a796c0a6b0, IBDA_ConditionalAccess);
        GUID_PAIR(400cc286-32a0-4ce4-9041-39571125a635, IMpeg2Stream);
        GUID_PAIR(cdf7be60-d954-42fd-a972-78971958e470, IDVBSTuningSpace);
        GUID_PAIR(0754cd31-8d15-47a9-8215-d20064157244, IPersistTuneXml);
        GUID_PAIR(ef7e0655-7888-4960-b0e5-730846e03492, IBackgroundCopyFile4);
        GUID_PAIR(01b7bd23-fb88-4a77-8490-5891d3e4653a, IBackgroundCopyFile);
        GUID_PAIR(34df6950-7fb6-11d0-8817-00a0c903b83c, ICertAdmin);
        GUID_PAIR(c7ea09c0-ce17-11d0-8833-00a0c903b83c, ICertGetConfig);
        GUID_PAIR(372fce34-4324-11d0-8810-00a0c903b83c, ICertConfig);
        GUID_PAIR(014e4840-5523-11d0-8812-00a0c903b83c, ICertRequest);
        GUID_PAIR(12a88820-7494-11d0-8816-00a0c903b83c, ICertEncodeStringArray);
        GUID_PAIR(15e2f230-a0a2-11d0-8821-00a0c903b83c, ICertEncodeLongArray);
        GUID_PAIR(2f9469a0-a470-11d0-8821-00a0c903b83c, ICertEncodeDateArray);
        GUID_PAIR(e19ae1a0-7364-11d0-8816-00a0c903b83c, ICertExit);
        GUID_PAIR(d99e6e70-fc88-11d0-b498-00a0c90312f3, ICertRequestD);
        GUID_PAIR(ee5ff7b3-4572-11d0-9452-00a0c905416e, ITransactionResource);
        GUID_PAIR(ac2b8ad2-d6f0-11d0-b386-00a0c9083365, IDtcLuRecovery);
        GUID_PAIR(ac2b8ad1-d6f0-11d0-b386-00a0c9083365, IDtcLuRecoveryInitiatedByLuWork);
        GUID_PAIR(04b0f1a7-9490-44bc-96e1-4296a31252e2, IFileOperationProgressSink);
        GUID_PAIR(2f0666c6-12f7-4360-b511-a394a0553725, ILaunchTargetViewSizePreference);
        GUID_PAIR(aa80e905-2021-11d2-93e0-0060b067b86e, ITextStoreAnchorSink);
        GUID_PAIR(01c2d285-d3c7-4b7b-b5b5-d97411d0c283, ITfLangBarItemBalloon);
        GUID_PAIR(aa80e801-2021-11d2-93e0-0060b067b86e, ITfThreadMgr);
        GUID_PAIR(aa80e808-2021-11d2-93e0-0060b067b86e, IEnumTfDocumentMgrs);
        GUID_PAIR(aa80e7f4-2021-11d2-93e0-0060b067b86e, ITfDocumentMgr);
        GUID_PAIR(aa80e803-2021-11d2-93e0-0060b067b86e, ITfEditSession);
        GUID_PAIR(aa80e901-2021-11d2-93e0-0060b067b86e, ITextStoreACPServices);
        GUID_PAIR(aa80e7f0-2021-11d2-93e0-0060b067b86e, ITfKeystrokeMgr);
        GUID_PAIR(aa80e7f5-2021-11d2-93e0-0060b067b86e, ITfKeyEventSink);
        GUID_PAIR(aa80e7f7-2021-11d2-93e0-0060b067b86e, ITfTextInputProcessor);
        GUID_PAIR(344488b7-6846-474b-b989-f027448245e0, ID3D12Debug);
        GUID_PAIR(1bc752e0-9046-11d1-b8b3-006008059382, IPublishedApp);
        GUID_PAIR(07250a10-9cf9-11d1-9076-006008059382, IAppPublisher);
        GUID_PAIR(fac32c80-cbe4-11ce-8350-444553540000, FolderItem);
        GUID_PAIR(744129e0-cbe5-11ce-8350-444553540000, FolderItems);
        GUID_PAIR(88a05c00-f000-11ce-8350-444553540000, IShellLinkDual);
        GUID_PAIR(cf8fc579-c396-4774-85f1-d908a831156e, ISyncMgrConflictStore);
        GUID_PAIR(0364eb00-4a77-11d1-a671-006097c9a2e8, ITAMMediaFormat);
        GUID_PAIR(79184a66-8664-423f-97f1-637356a5d812, ITaskFolderCollection);
        GUID_PAIR(def8f855-0b49-4087-b1a9-b8b38488f786, IEventTrigger);
        GUID_PAIR(c241f5e2-7206-11d8-a2c7-00a0d1d6c6b3, ITextRange2);
        GUID_PAIR(c241f5e1-7206-11d8-a2c7-00a0d1d6c6b3, ITextSelection2);
        GUID_PAIR(c241f5e3-7206-11d8-a2c7-00a0d1d6c6b3, ITextFont2);
        GUID_PAIR(c241f5e4-7206-11d8-a2c7-00a0d1d6c6b3, ITextPara2);
        GUID_PAIR(c241f5e5-7206-11d8-a2c7-00a0d1d6c6b3, ITextStoryRanges2);
        GUID_PAIR(c241f5f3-7206-11d8-a2c7-00a0d1d6c6b3, ITextStory);
        GUID_PAIR(c241f5e7-7206-11d8-a2c7-00a0d1d6c6b3, ITextStrings);
        GUID_PAIR(c241f5f2-7206-11d8-a2c7-00a0d1d6c6b3, ITextDisplays);
        GUID_PAIR(6b0efad1-a053-41d6-9085-33a689144665, IUIAnimationTimer);
        GUID_PAIR(d014e196-0e50-4843-a5d2-c22897c8845a, IMultipleViewProvider_win10);
        GUID_PAIR(f4efea88-65b1-4f30-a4b9-2793987796fb, IWdsTransportSession);
        GUID_PAIR(37d38bb6-3ee9-4110-9312-14b194163337, IXpsOMPath);
        GUID_PAIR(15b873d5-1971-41e8-83a3-6578403064c7, IXpsOMThumbnailGenerator);
        GUID_PAIR(332e5848-2e15-458e-85c4-c911c0c3d6f4, IRandomAccessStreamFileAccessMode);
        GUID_PAIR(0dc38853-c1b0-4176-a984-b298361027af, IEnumDebugStackFrames64);
        GUID_PAIR(370af178-7758-4dad-8146-7391f6e18585, IAppHostConfigLocation);
        GUID_PAIR(06210e88-01f5-11d1-b512-0080c781c384, IService);
        GUID_PAIR(2a1c53c4-8638-4b3e-b518-2773c94556a3, IPrinterScriptablePropertyBag2);
        GUID_PAIR(ec0671b3-1b78-4b80-a464-9132247543e3, IRDPSRAPIAttendee);
        GUID_PAIR(05e12f95-28b3-4c9a-8780-d0248574a1e0, IRDPSRAPIVirtualChannel);
        GUID_PAIR(a072f1d6-0286-4e1f-85f2-17a2948456ec, IRTCSessionPortManagement);
        GUID_PAIR(a1533d18-0ac1-4084-85b9-89a96116806b, ID3D12ProtectedSession);
        GUID_PAIR(85611e73-70a9-490e-9614-a9e302777904, ID3D11On12Device);
        GUID_PAIR(09b5eab0-f997-11d1-93d4-0060b067b86e, IEnumInputContext);
        GUID_PAIR(08c0e040-62d1-11d1-9326-0060b067b86e, IActiveIMMApp);
        GUID_PAIR(18e7e422-2467-4e1c-a459-d8a29303d092, IAppointmentCalendar2);
        GUID_PAIR(33ee3134-1dd6-4e3a-8067-d1c162e8642b, IRandomAccessStreamReference);
        GUID_PAIR(c241f5e0-7206-11d8-a2c7-00a0d1d6c6b3, ITextDocument2);
        GUID_PAIR(2f5f00f2-2c18-4f88-8435-c534086076f5, ICmsTimestampInfo);
        GUID_PAIR(a5783dd5-58e3-4efb-b283-a1653126e1be, IKeyDerivationParametersStatics2);
        GUID_PAIR(4a01eb05-4e42-41d4-b518-e008a5163614, IWebAccountProvider2);
        GUID_PAIR(cfb2a855-1c59-4b13-b1e5-16e29805f7c4, ICoreWindowDialogFactory);
        GUID_PAIR(60840774-3da0-4949-8200-7206c00190a0, ICompositionEasingFunctionFactory);
        GUID_PAIR(97909e87-9291-4f91-b6c8-b6e359d7a7fb, IUnicodeCharactersStatics);
        GUID_PAIR(59a5aa65-fa21-4ffc-b155-04d928012686, IGattServiceProviderAdvertisementStatusChangedEventArgs);
        GUID_PAIR(da4b7cf0-8d71-446f-88c3-b95184a2d6cd, IControlChannelTriggerFactory);
        GUID_PAIR(cf5b7ec1-9791-4453-b013-29b6f778e519, IDisplayEnhancementOverrideStatics);
        GUID_PAIR(8c7ce857-a7e7-489d-b9d6-368061515012, ISmartCardCryptogramGetAllCryptogramStorageKeyCharacteristicsResult);
        GUID_PAIR(c3d9cbe0-3ff6-4fb3-b262-50d435c55423, IAudioGraphUnrecoverableErrorOccurredEventArgs);
        GUID_PAIR(de721c83-4a43-4ad1-a6d2-2492a796c3f2, ICastingDevice);
        GUID_PAIR(f429ea72-3467-47e6-a027-522923e9d727, ICastingSource);
        GUID_PAIR(7133fce1-1769-4ff9-a7c1-38d2c4d42360, IMediaPlaybackItemFactory);
        GUID_PAIR(4c6d20f4-9457-44e6-a323-d122b958780b, IHttpMediaTypeWithQualityHeaderValueFactory);
        GUID_PAIR(1c741d59-2122-47d5-a856-83f3d4214875, IXmlNode);
        GUID_PAIR(f77d6fa4-4652-410e-b1d8-e9a5e245a45c, IMediaSourceStatics);
        GUID_PAIR(05889117-b3e1-36d8-b017-566181a5b196, ISpatialBoundingVolumeStatics);
        GUID_PAIR(bdbe9ce3-3985-4e72-97c2-0590d61307a1, IFrameExposureCapabilities);
        GUID_PAIR(c0d889f6-5228-4453-b153-85606592b238, IFocusControl);
        GUID_PAIR(e4c94395-2028-48ed-90b0-d1c1b162e24c, IMediaFrameReader);
        GUID_PAIR(d269d1f1-05a7-49ae-87d7-16b2b8b9a553, ISystemConditionFactory);
        GUID_PAIR(858fa073-1e19-4105-b2f7-c8478808d562, IOperationCompletedEventArgs2);
        GUID_PAIR(6a213c04-6614-49f8-99a2-b5e9b3a085c8, ISpeechContinuousRecognitionSession);
        GUID_PAIR(4aee9d91-bf15-4da9-95f6-f6b7d558225b, ITargetFileRequestDeferral);
        GUID_PAIR(03b11a58-5276-4d91-8621-54611864d5fa, IStartupTaskActivatedEventArgs);
        GUID_PAIR(cf7f59b3-6a09-4de8-a6c0-5792d56880d1, IAppInfo);
        GUID_PAIR(ae719709-1a52-4321-87b3-e5a1a17981a7, IPackageCatalogRemoveResourcePackagesResult);
        GUID_PAIR(cd7b4228-fd74-443e-b114-23e677b0e86f, IPackageUpdatingEventArgs);
        GUID_PAIR(1d9ecbe4-3fe8-4335-8392-93d8e36865f0, IJsonValueStatics2);
        GUID_PAIR(df9a26c6-e746-4bcd-b5d4-120103c4209b, IUser);
        GUID_PAIR(651b9ad0-0fc7-4aa9-9538-d89931010741, IMediaObjectInPlace);
        GUID_PAIR(88e04db3-1012-4d64-9996-f703a950d3f4, IPortableDeviceDataStream);
        GUID_PAIR(66e11784-f695-4f28-a505-a7080081a78f, IAudioEndpointVolumeEx);
        GUID_PAIR(fb2b72a0-7a68-11d1-88f9-0080c7d771bf, IEventClass);
        GUID_PAIR(fb2b72a1-7a68-11d1-88f9-0080c7d771bf, IEventClass2);
        GUID_PAIR(ad4c1b00-4bf7-422f-9175-756693d9130d, IMFByteStream);
        GUID_PAIR(3c9b2eb9-86d5-4514-a394-f56664f9f0d8, IMFMediaSourceEx);
        GUID_PAIR(56f14771-8677-484f-a170-e386e418a676, ISyncChangeBuilder);
        GUID_PAIR(23fee831-ae96-42df-b170-25a04847a3ca, ITocCollection);
        GUID_PAIR(1d1b5ae0-42d9-4021-8261-3987619512e9, IWSManEx2);
        GUID_PAIR(f1d70332-d070-4ef1-a088-78313536c2d6, IWRdsProtocolConnectionCallback);
        GUID_PAIR(879ff725-1a08-4aca-a075-3355126312e6, IChatMessageTransportConfiguration);
        GUID_PAIR(b4d32d06-2332-4678-8378-28b579336846, IEmailMailboxDeleteFolderRequestEventArgs);
        GUID_PAIR(7696fe55-d5d3-4d3d-b345-45591759c510, IPaymentCanMakePaymentResult);
        GUID_PAIR(6f491df4-32d6-4b40-b474-b83038a4d9cf, IProductPurchaseDisplayPropertiesFactory);
        GUID_PAIR(7cab6508-8ec4-42fd-a5a5-e4b7d1d5316d, IInkDrawingAttributes2);
        GUID_PAIR(7c925755-3e48-42b4-8677-76372267033f, ICustomPropertyProvider);
        GUID_PAIR(961661f9-8bb4-4e6c-b923-b5d787e0f1a9, IGradientBrushStatics);
        GUID_PAIR(9fcab141-04c0-4afb-87b3-e800b969b894, ILineSegmentStatics);
        GUID_PAIR(c18606a6-39f4-4b8a-8403-28e5e5f033b4, IMatrixHelperStatics);
        GUID_PAIR(389b7c71-5220-42b2-9992-2690c1a6702f, IButtonBaseFactory);
        GUID_PAIR(5eefe579-c706-4781-a308-c9e7f4c6a1d7, IDragStartedEventArgsFactory);
        GUID_PAIR(389b7c71-5220-42b2-9992-2690c1a67030, IRangeBaseFactory);
        GUID_PAIR(b51d82d9-0910-4589-a284-b0c9205858e9, IColorKeyFrame);
        GUID_PAIR(196938c1-1c07-4c28-8847-f9f055b32855, IPopInThemeAnimation);
        GUID_PAIR(ef8814a6-9388-4aca-8572-405194069080, IPageStackEntry);
        GUID_PAIR(5a105887-e494-4d2b-8528-39472296133f, IBitmapIconSourceStatics);
        GUID_PAIR(b81d1639-e911-4fd7-a4c4-b9c7f008b7fc, IBorderStatics);
        GUID_PAIR(32233e36-ab74-4a25-a556-45293135b2c5, IControlStatics2);
        GUID_PAIR(1a9c6e99-1719-4b80-b335-628331a07019, IListViewBase5);
        GUID_PAIR(8532b8f7-3e16-455b-89a8-c1346223f6d3, IListViewBaseStatics);
        GUID_PAIR(f9b56047-8da6-4730-9208-144125c6b396, IMenuFlyoutItemBase);
        GUID_PAIR(6024d9d1-56b7-41f0-9558-3934c14244d6, IPasswordBox3);
        GUID_PAIR(0c89d500-e8d8-4eae-8804-33649205d7d2, IRichEditBox6);
        GUID_PAIR(d1113f67-d8c1-4ae4-98f0-d8504502f08b, IStyleSelector);
        GUID_PAIR(c04c8ed3-9af2-4e75-a329-7497a110e7a8, ITreeViewNode);
        GUID_PAIR(162ac829-7115-43ec-b383-a7b71644069d, ISelectorAutomationPeer);
        GUID_PAIR(d2dd9ed0-d3c5-4056-91b5-b7c1d1e8130e, ISoftwareBitmapSource);
        GUID_PAIR(48d37585-e1a6-469b-83c8-30825037119e, IDebugSettings2);
        GUID_PAIR(17d2ad90-1370-55c8-80e1-78b49004a9e1, IElementFactory);
        GUID_PAIR(7a826ce1-07a0-4083-b6d1-b1d917b976ac, IGridLengthHelper);
        GUID_PAIR(677112a7-50d4-496c-8415-0602b4c6d3bf, IRegisterPackageOptions);
        GUID_PAIR(ad342654-57e8-453e-8329-d1e447d103a9, II2cDeviceProvider);
        GUID_PAIR(341de352-1d33-40a1-a1d3-e5777020e1f1, IAllJoynSessionMemberAddedEventArgsFactory);
        GUID_PAIR(7ace9787-c8a0-4e95-a1e0-d58676538649, ILampArray);
        GUID_PAIR(7669cc95-4640-461c-8158-16f65265d77c, IVsTextEditorPropertyContainer);
        GUID_PAIR(19404d57-f8e4-42f4-9255-b8f889b0c50c, IVsLanguageContextProvider);
        GUID_PAIR(c08e5275-0d26-4de9-8892-994024c23750, IVsTextBuffer);
        GUID_PAIR(d7820968-4cf8-4760-b633-570282e5f66d, IVsDropdownBar);
        GUID_PAIR(978a8e17-4df8-432a-9623-d530a26452bc, IVsUserData);
        GUID_PAIR(0c71e351-5141-4b6f-b838-e212f0764875, IVsSyntheticTextSession);
        GUID_PAIR(59b2d1d0-5db0-4f9f-9609-13f0168516d6, IVsHierarchy);
        GUID_PAIR(bec77711-2df9-44d7-b478-a453c2e8a134, IEnumHierarchies);
        GUID_PAIR(a5412570-5fb9-11d1-a811-00a0c9110051, IVsDebugLaunch);
        GUID_PAIR(65bc5c20-41d1-11d1-84a4-00a0c9110055, IVsLaunchPadEvents);
        GUID_PAIR(c21c16a2-1612-4995-b445-f7b1c1657878, IVsLaunchPadFactory);
        GUID_PAIR(76a3b2c0-c743-11d0-a7a6-00a0c9110051, IVsTextBufferProvider);
        GUID_PAIR(bed89b98-6ec9-43cb-b0a8-41d6e2d6669d, IVsGeneratorProgress);
        GUID_PAIR(7ff85070-4667-4532-b149-63a7b205060b, IVsFindSymbol);
        GUID_PAIR(76af73f9-a322-42b0-a515-d4d7553508fe, IVsActivityLog);
        GUID_PAIR(8e40d748-f682-4951-b465-16d0c252a69d, IVsProjectDataConnection);
        GUID_PAIR(7ff85072-4667-4532-b149-63a7b205060b, IVsFindSymbol2);
        GUID_PAIR(2efc69a8-5e06-436d-88d5-f099353356da, IVsToolWindowToolbarHost2);
        GUID_PAIR(cb218890-1382-472b-9118-782700c88115, TextDocument);
        GUID_PAIR(509b9955-7303-48c9-90d4-e165b974e6ba, CommandWindow);
        GUID_PAIR(7b076ad1-bd51-11d2-9238-00a02448799a, IEnumDebugENCInfo2);
        GUID_PAIR(e51be743-bd57-11d2-9238-00a02448799a, IEnumDebugENCRelinkInfo2);
        GUID_PAIR(9b7de9a9-bd59-11d2-9238-00a02448799a, IDebugIDBInfo2);
        GUID_PAIR(0ebf1959-bd57-11d2-9238-00a02448799a, IDebugENCUpdateOnRelinkEvent2);
        GUID_PAIR(2f01eb29-bd57-11d2-9238-00a02448799a, IDebugENCUpdateOnStaleCodeEvent2);
        GUID_PAIR(e147e9e3-6440-4073-a7b7-a65592c714b5, IDebugProgramDestroyEvent2);
        GUID_PAIR(ef7262c4-4a01-42a0-8658-932667b27555, IEnumDebugSessionFrameInfo2);
        GUID_PAIR(e2e3aee2-9e39-40e5-8609-d43260630766, IDebugUserNotificationUI120);
        GUID_PAIR(15c89f68-17d2-4c34-9714-e5a1e0824904, IAsyncDebugGetFramesCompletionHandler);
        GUID_PAIR(7a6335c8-7884-11d0-a8a9-00a0c921a4d2, IEnumActions);
        GUID_PAIR(09ccd272-5fa0-11d2-b1f8-0080c747d9a0, IVsDispatch);
        GUID_PAIR(f12c3b51-b3c2-42ea-86b0-6185c6f712a8, IVxNormalizedSpanCollection);
        GUID_PAIR(d058cf17-7aa6-41da-8816-90b93250e998, IVxProjectionSourceBuffersChangedEventArgs);
        GUID_PAIR(1fc6af83-7f43-467e-b2c4-28e2b1b376ab, IVsPackageExtensionProvider);
        GUID_PAIR(30525828-bd80-4bdf-9255-d1e0e1c0f34f, IVsRunningDocumentTable3);
        GUID_PAIR(44cec4a2-7148-4044-b836-678374e296f2, IVsAggregatableProject2);
        GUID_PAIR(bd868c04-7979-4db0-a2e1-f55490a42640, IVsMenuEvents);
        GUID_PAIR(fdcad245-e468-48f3-b603-2641e020d6d0, CodeFolders);
        GUID_PAIR(03e26311-4f76-11d3-88c6-006097945418, ICorDebugILFrame);
        GUID_PAIR(03e26314-4f76-11d3-88c6-006097945418, ICorDebugNativeFrame);
        GUID_PAIR(ecc8691b-c1db-4dc0-855e-65f6c551af49, INoMarshal);

        GUID_PAIR(34a715a0-6587-11d0-924a-0020afc7ac4d, DWebBrowserEvents2);
        GUID_PAIR(6d5140c1-7436-11ce-8034-00aa006009fa, IServiceProvider);
        GUID_PAIR(b722bccb-4e68-101b-a2bc-00aa00404770, IOleCommandTarget);
        GUID_PAIR(b196b289-bab4-101a-b69c-00aa00341d07, IOleControlSite);
        GUID_PAIR(bd3f23c0-d43e-11cf-893b-00aa00bdce1a, IDocHostUIHandler);
        GUID_PAIR(3050f6d0-98b5-11cf-bb82-00aa00bdce0b, IDocHostUIHandler2);
        GUID_PAIR(c4d244b0-d43e-11cf-893b-00aa00bdce1a, DocHostShowUI);
        GUID_PAIR(9c2cad80-3424-11cf-b670-00aa004cd6d8, IOleInPlaceSiteEx);
        GUID_PAIR(b6ea2050-048a-11d1-82b9-00c04fb9942e, IAxWinHostWindow);
// 00000008-0000-0000-c000-000000000046, IProxyManager?
// 30510406-98b5-11cf-bb82-00aa00bdce0b
        GUID_PAIR(a6ef9860-c720-11d0-9337-00a0c90dcaa9, IDispatchEx);
// 9bcb0016-bc2a-47b7-8154-8580a15c3ff0
        GUID_PAIR(719c3050-f9d3-11cf-a493-00400523a8a0, INameTbl);
        GUID_PAIR(a0aac450-a77b-11cf-91d0-00aa00c14a7c, IDispatchEx1);

        // oaidl.h
        GUID_PAIR(00020405-0000-0000-C000-000000000046, ICreateTypeInfo);
        GUID_PAIR(0002040E-0000-0000-C000-000000000046, ICreateTypeInfo2);
        GUID_PAIR(00020406-0000-0000-C000-000000000046, ICreateTypeLib);
        GUID_PAIR(0002040F-0000-0000-C000-000000000046, ICreateTypeLib2);
        GUID_PAIR(00020403-0000-0000-C000-000000000046, ITypeComp);
        GUID_PAIR(00020404-0000-0000-c000-000000000046, IEnumVARIANT);
        GUID_PAIR(00020401-0000-0000-C000-000000000046, ITypeInfo);
        GUID_PAIR(00020412-0000-0000-C000-000000000046, ITypeInfo2);
        GUID_PAIR(00020402-0000-0000-C000-000000000046, ITypeLib);
        GUID_PAIR(00020411-0000-0000-C000-000000000046, ITypeLib2);
        GUID_PAIR(00020410-0000-0000-C000-000000000046, ITypeChangeEvent);
        GUID_PAIR(1CF2B120-547D-101B-8E65-08002B2BD119, IErrorInfo);
        GUID_PAIR(22F03340-547D-101B-8E65-08002B2BD119, ICreateErrorInfo);
        GUID_PAIR(DF0B3D60-548F-101B-8E65-08002B2BD119, ISupportErrorInfo);
        GUID_PAIR(0000002E-0000-0000-C000-000000000046, ITypeFactory);
        GUID_PAIR(0000002D-0000-0000-C000-000000000046, ITypeMarshal);
        GUID_PAIR(0000002F-0000-0000-C000-000000000046, IRecordInfo);
        GUID_PAIR(3127CA40-446E-11CE-8135-00AA004BB851, IErrorLog);
        GUID_PAIR(55272A00-42CB-11CE-8135-00AA004BB851, IPropertyBag);
        GUID_PAIR(ED6A8A2A-B160-4E77-8F73-AA7435CD5C27, ITypeLibRegistrationReader);
        GUID_PAIR(76A3E735-02DF-4A12-98EB-043AD3600AF3, ITypeLibRegistration);

        // direct show - control
        GUID_PAIR(56a868b9-0ad4-11ce-b03a-0020af0ba770, IAMCollection);
        GUID_PAIR(56a868b1-0ad4-11ce-b03a-0020af0ba770, IMediaControl);
        GUID_PAIR(56a868b6-0ad4-11ce-b03a-0020af0ba770, IMediaEvent);
        GUID_PAIR(56a868c0-0ad4-11ce-b03a-0020af0ba770, IMediaEventEx);
        GUID_PAIR(56a868b2-0ad4-11ce-b03a-0020af0ba770, IMediaPosition);
        GUID_PAIR(56a868b3-0ad4-11ce-b03a-0020af0ba770, IBasicAudio);
        GUID_PAIR(56a868b4-0ad4-11ce-b03a-0020af0ba770, IVideoWindow);
        GUID_PAIR(56a868b5-0ad4-11ce-b03a-0020af0ba770, IBasicVideo);
        GUID_PAIR(329bb360-f6ea-11d1-9038-00a0c9697298, IBasicVideo2);
        GUID_PAIR(56a868b8-0ad4-11ce-b03a-0020af0ba770, IDeferredCommand);
        GUID_PAIR(56a868b7-0ad4-11ce-b03a-0020af0ba770, IQueueCommand);
        GUID_PAIR(56a868ba-0ad4-11ce-b03a-0020af0ba770, IFilterInfo);
        GUID_PAIR(56a868bb-0ad4-11ce-b03a-0020af0ba770, IRegFilterInfo);
        GUID_PAIR(56a868bc-0ad4-11ce-b03a-0020af0ba770, IMediaTypeInfo);
        GUID_PAIR(56a868bd-0ad4-11ce-b03a-0020af0ba770, IPinInfo);
        GUID_PAIR(bc9bcf80-dcd2-11d2-abf6-00a0c905f375, IAMStats);
        GUID_PAIR(256A6A21-FBAD-11d1-82BF-00A0C9696C8F, IAMVideoAcceleratorNotify);
        GUID_PAIR(256A6A22-FBAD-11d1-82BF-00A0C9696C8F, IAMVideoAccelerator);
            
        // direct show - strmif
        GUID_PAIR(29840822-5B84-11D0-BD3B-00A0C911CE86, ICreateDevEnum);
        GUID_PAIR(56a86891-0ad4-11ce-b03a-0020af0ba770, IPin);
        GUID_PAIR(56a86892-0ad4-11ce-b03a-0020af0ba770, IEnumPins);
        GUID_PAIR(89c31040-846b-11ce-97d3-00aa0055595a, IEnumMediaTypes);
        GUID_PAIR(56a8689f-0ad4-11ce-b03a-0020af0ba770, IFilterGraph);
        GUID_PAIR(56a86893-0ad4-11ce-b03a-0020af0ba770, IEnumFilters);
        GUID_PAIR(56a86899-0ad4-11ce-b03a-0020af0ba770, IMediaFilter);
        GUID_PAIR(56a86895-0ad4-11ce-b03a-0020af0ba770, IBaseFilter);
        GUID_PAIR(56a86897-0ad4-11ce-b03a-0020af0ba770, IReferenceClock);
        GUID_PAIR(36b73885-c2c8-11cf-8b46-00805f6cef60, IReferenceClock2);
        GUID_PAIR(56a8689a-0ad4-11ce-b03a-0020af0ba770, IMediaSample);
        GUID_PAIR(36b73884-c2c8-11cf-8b46-00805f6cef60, IMediaSample2);
        GUID_PAIR(56a8689c-0ad4-11ce-b03a-0020af0ba770, IMemAllocator);
        GUID_PAIR(379a0cf0-c1de-11d2-abf5-00a0c905f375, IMemAllocatorCallbackTemp);
        GUID_PAIR(92980b30-c1de-11d2-abf5-00a0c905f375, IMemAllocatorNotifyCallbackTemp);
        GUID_PAIR(56a8689d-0ad4-11ce-b03a-0020af0ba770, IMemInputPin);
        GUID_PAIR(a3d8cec0-7e5a-11cf-bbc5-00805f6cef20, IAMovieSetup);
        GUID_PAIR(36b73880-c2c8-11cf-8b46-00805f6cef60, IMediaSeeking);
        GUID_PAIR(56a868a4-0ad4-11ce-b03a-0020af0ba770, IEnumRegFilters);
        GUID_PAIR(56a868a3-0ad4-11ce-b03a-0020af0ba770, IFilterMapper);
        GUID_PAIR(b79bb0b0-33c1-11d1-abe1-00a0c905f375, IFilterMapper2);
        GUID_PAIR(b79bb0b1-33c1-11d1-abe1-00a0c905f375, IFilterMapper3);
        GUID_PAIR(56a868a5-0ad4-11ce-b03a-0020af0ba770, IQualityControl);
        GUID_PAIR(56a868a0-0ad4-11ce-b03a-0020af0ba770, IOverlayNotify);
        GUID_PAIR(680EFA10-D535-11D1-87C8-00A0C9223196, IOverlayNotify2);
        GUID_PAIR(56a868a1-0ad4-11ce-b03a-0020af0ba770, IOverlay);
        GUID_PAIR(56a868a2-0ad4-11ce-b03a-0020af0ba770, IMediaEventSink);
        GUID_PAIR(56a868a6-0ad4-11ce-b03a-0020af0ba770, IFileSourceFilter);
        GUID_PAIR(a2104830-7c70-11cf-8bce-00aa00a3f1a6, IFileSinkFilter);
        GUID_PAIR(00855B90-CE1B-11d0-BD4F-00A0C911CE86, IFileSinkFilter2);
        GUID_PAIR(56a868a9-0ad4-11ce-b03a-0020af0ba770, IGraphBuilder);
        GUID_PAIR(bf87b6e0-8c27-11d0-b3f0-00aa003761c5, ICaptureGraphBuilder);
        GUID_PAIR(670d1d20-a068-11d0-b3f0-00aa003761c5, IAMCopyCaptureFileProgress);
        GUID_PAIR(93E5A4E0-2D50-11d2-ABFA-00A0C9C6E38D, ICaptureGraphBuilder2);
        GUID_PAIR(36b73882-c2c8-11cf-8b46-00805f6cef60, IFilterGraph2);
        GUID_PAIR(56a868bf-0ad4-11ce-b03a-0020af0ba770, IStreamBuilder);
        GUID_PAIR(56a868aa-0ad4-11ce-b03a-0020af0ba770, IAsyncReader);
        GUID_PAIR(56a868ab-0ad4-11ce-b03a-0020af0ba770, IGraphVersion);
        GUID_PAIR(56a868ad-0ad4-11ce-b03a-0020af0ba770, IResourceConsumer);
        GUID_PAIR(56a868ac-0ad4-11ce-b03a-0020af0ba770, IResourceManager);
        GUID_PAIR(56a868af-0ad4-11ce-b03a-0020af0ba770, IDistributorNotify);
        GUID_PAIR(36b73881-c2c8-11cf-8b46-00805f6cef60, IAMStreamControl);
        GUID_PAIR(36b73883-c2c8-11cf-8b46-00805f6cef60, ISeekingPassThru);
        GUID_PAIR(C6E13340-30AC-11d0-A18C-00A0C9118956, IAMStreamConfig);
        GUID_PAIR(BEE3D220-157B-11d0-BD23-00A0C911CE86, IConfigInterleaving);
        GUID_PAIR(5ACD6AA0-F482-11ce-8B67-00AA00A3F1A6, IConfigAviMux);
        GUID_PAIR(C6E13343-30AC-11d0-A18C-00A0C9118956, IAMVideoCompression);
        GUID_PAIR(D8D715A0-6E5E-11D0-B3F0-00AA003761C5, IAMVfwCaptureDialogs);
        GUID_PAIR(D8D715A3-6E5E-11D0-B3F0-00AA003761C5, IAMVfwCompressDialogs);
        GUID_PAIR(C6E13344-30AC-11d0-A18C-00A0C9118956, IAMDroppedFrames);
        GUID_PAIR(54C39221-8380-11d0-B3F0-00AA003761C5, IAMAudioInputMixer);
        GUID_PAIR(56ED71A0-AF5F-11D0-B3F0-00AA003761C5, IAMBufferNegotiation);
        GUID_PAIR(C6E13350-30AC-11d0-A18C-00A0C9118956, IAMAnalogVideoDecoder);
        GUID_PAIR(C6E13360-30AC-11d0-A18C-00A0C9118956, IAMVideoProcAmp);
        GUID_PAIR(C6E13370-30AC-11d0-A18C-00A0C9118956, IAMCameraControl);
        GUID_PAIR(6a2e0670-28e4-11d0-a18c-00a0c9118956, IAMVideoControl);
        GUID_PAIR(C6E13380-30AC-11d0-A18C-00A0C9118956, IAMCrossbar);
        GUID_PAIR(211A8761-03AC-11d1-8D13-00AA00BD8339, IAMTuner);
        GUID_PAIR(211A8760-03AC-11d1-8D13-00AA00BD8339, IAMTunerNotification);
        GUID_PAIR(211A8766-03AC-11d1-8D13-00AA00BD8339, IAMTVTuner);
        GUID_PAIR(211A8765-03AC-11d1-8D13-00AA00BD8339, IBPCSatelliteTuner);
        GUID_PAIR(83EC1C30-23D1-11d1-99E6-00A0C9560266, IAMTVAudio);
        GUID_PAIR(83EC1C33-23D1-11d1-99E6-00A0C9560266, IAMTVAudioNotification);
        GUID_PAIR(C6E133B0-30AC-11d0-A18C-00A0C9118956, IAMAnalogVideoEncoder);
        GUID_PAIR(31EFAC30-515C-11d0-A9AA-00AA0061BE93, IKsPropertySet);
        GUID_PAIR(6025A880-C0D5-11d0-BD4E-00A0C911CE86, IMediaPropertyBag);
        GUID_PAIR(5738E040-B67F-11d0-BD4D-00A0C911CE86, IPersistMediaPropertyBag);
        GUID_PAIR(F938C991-3029-11cf-8C44-00AA006B6814, IAMPhysicalPinInfo);
        GUID_PAIR(B5730A90-1A2C-11cf-8C23-00AA006B6814, IAMExtDevice);
        GUID_PAIR(A03CD5F0-3045-11cf-8C44-00AA006B6814, IAMExtTransport);
        GUID_PAIR(9B496CE1-811B-11cf-8C77-00AA006B6814, IAMTimecodeReader);
        GUID_PAIR(9B496CE0-811B-11cf-8C77-00AA006B6814, IAMTimecodeGenerator);
        GUID_PAIR(9B496CE2-811B-11cf-8C77-00AA006B6814, IAMTimecodeDisplay);
        GUID_PAIR(c6545bf0-e76b-11d0-bd52-00a0c911ce86, IAMDevMemoryAllocator);
        GUID_PAIR(c6545bf1-e76b-11d0-bd52-00a0c911ce86, IAMDevMemoryControl);
        GUID_PAIR(c1960960-17f5-11d1-abe1-00a0c905f375, IAMStreamSelect);
        GUID_PAIR(8389d2d0-77d7-11d1-abe6-00a0c905f375, IAMResourceControl);
        GUID_PAIR(4d5466b0-a49c-11d1-abe8-00a0c905f375, IAMClockAdjust);
        GUID_PAIR(2dd74950-a890-11d1-abe8-00a0c905f375, IAMFilterMiscFlags);
        GUID_PAIR(56a868fd-0ad4-11ce-b0a3-0020af0ba770, IAMFilterGraphCallback);
        GUID_PAIR(48efb120-ab49-11d2-aed2-00a0c995e8d5, IDrawVideoImage);
        GUID_PAIR(2e5ea3e0-e924-11d2-b6da-00a0c995e8df, IDecimateVideoImage);
        GUID_PAIR(60d32930-13da-11d3-9ec6-c4fcaef5c7be, IAMVideoDecimationProperties);
        GUID_PAIR(e46a9787-2b71-444d-a4b5-1fab7b708d6a, IVideoFrameStep);
        GUID_PAIR(62EA93BA-EC62-11d2-B770-00C04FB6BD3D, IAMLatency);
        GUID_PAIR(F185FE76-E64E-11d2-B76E-00C04FB6BD3D, IAMPushSource);
        GUID_PAIR(f90a6130-b658-11d2-ae49-0000f8754b99, IAMDeviceRemoval);
        GUID_PAIR(d18e17a0-aacb-11d0-afb0-00aa00b67a42, IDVEnc);
        GUID_PAIR(b8e8bd60-0bfe-11d0-af91-00aa00b67a42, IIPDVDec);
        GUID_PAIR(58473A19-2BC8-4663-8012-25F81BABDDD1, IDVRGB219);
        GUID_PAIR(92a3a302-da7c-4a1f-ba7e-1802bb5d2d02, IDVSplitter);
        GUID_PAIR(22320CB2-D41A-11d2-BF7C-D7CB9DF0BF93, IAMAudioRendererStats);
        GUID_PAIR(632105FA-072E-11d3-8AF9-00C04FB6BD3D, IAMGraphStreams);
        GUID_PAIR(62fae250-7e65-4460-bfc9-6398b322073c, IAMOverlayFX);
        GUID_PAIR(8E1C39A1-DE53-11cf-AA63-0080C744528D, IAMOpenProgress);
        GUID_PAIR(436eee9c-264f-4242-90e1-4e330c107512, IMpeg2Demultiplexer);
        GUID_PAIR(945C1566-6202-46fc-96C7-D87F289C6534, IEnumStreamIdMap);
        GUID_PAIR(D0E04C47-25B8-4369-925A-362A01D95444, IMPEG2StreamIdMap);
        GUID_PAIR(4995f511-9ddb-4f12-bd3b-f04611807b79, IAMGraphBuilderCallback);
        GUID_PAIR(4a9a62d3-27d4-403d-91e9-89f540e55534, IPinConnection);
        GUID_PAIR(c56e9858-dbf3-4f6b-8119-384af2060deb, IPinFlowControl);
        GUID_PAIR(03A1EB8E-32BF-4245-8502-114D08A9CB88, IGraphConfig);
        GUID_PAIR(ade0fd60-d19d-11d2-abf6-00a0c905f375, IGraphConfigCallback);
        GUID_PAIR(CE704FE7-E71E-41fb-BAA2-C4403E1182F5, IVMRImagePresenter);
        GUID_PAIR(31ce832e-4484-458b-8cca-f4d7e3db0b52, IVMRSurfaceAllocator);
        GUID_PAIR(aada05a8-5a4e-4729-af0b-cea27aed51e2, IVMRSurfaceAllocatorNotify);
        GUID_PAIR(0eb1088c-4dcd-46f0-878f-39dae86a51b7, IVMRWindowlessControl);
        GUID_PAIR(1c1a17b0-bed0-415d-974b-dc6696131599, IVMRMixerControl);
        GUID_PAIR(9cf0b1b6-fbaa-4b7f-88cf-cf1f130a0dce, IVMRMonitorConfig);
        GUID_PAIR(9e5530c5-7034-48b4-bb46-0b8a6efc8e36, IVMRFilterConfig);
        GUID_PAIR(ede80b5c-bad6-4623-b537-65586c9f8dfd, IVMRAspectRatioControl);
        GUID_PAIR(bb057577-0db8-4e6a-87a7-1a8c9a505a0f, IVMRDeinterlaceControl);
        GUID_PAIR(1E673275-0257-40aa-AF20-7C608D4A0428, IVMRMixerBitmap);
        GUID_PAIR(7a4fb5af-479f-4074-bb40-ce6722e43c82, IVMRImageCompositor);
        GUID_PAIR(058d1f11-2a54-4bef-bd54-df706626b727, IVMRVideoStreamControl);
        GUID_PAIR(a9849bbe-9ec8-4263-b764-62730f0d15d0, IVMRSurface);
        GUID_PAIR(9f3a1c85-8555-49ba-935f-be5b5b29d178, IVMRImagePresenterConfig);
        GUID_PAIR(e6f7ce40-4673-44f1-8f77-5499d68cb4ea, IVMRImagePresenterExclModeConfig);
        GUID_PAIR(1BD0ECB0-F8E2-11CE-AAC6-0020AF0B99A3, IQualProp);
        GUID_PAIR(a38d9567-5a9c-4f3c-b293-8eb415b279ba, IMFVideoDeviceID);
        GUID_PAIR(A5C6C53F-C202-4AA5-9695-175BA8C508A5, IMFVideoMixerControl);
        GUID_PAIR(073cd2fc-6cf4-40b7-8859-e89552c841f8, MR_VIDEO_MIXER_SERVICE);
        GUID_PAIR(1092a86c-ab1a-459a-a336-831fbc4d11ff, MR_VIDEO_RENDER_SERVICE);
        GUID_PAIR(efef5175-5c7d-4ce2-bbbd-34ff8bca6554, MR_VIDEO_ACCELERATION_SERVICE);
        GUID_PAIR(a0cade0f-06d5-4cf4-a1c7-f3cdd725aa75, IDirect3DDeviceManager9);
        GUID_PAIR(FA993888-4383-415A-A930-DD472A8CF6F7, IMFGetService);
        GUID_PAIR(fa99388a-4383-415a-a930-dd472a8cf6f7, IMFTopologyServiceLookupClient);

        //VMR9
        GUID_PAIR(69188c61-12a3-40f0-8ffc-342e7b433fd7, IVMRImagePresenter9);
        GUID_PAIR(8d5148ea-3f5d-46cf-9df1-d1b896eedb1f, IVMRSurfaceAllocator9);
        GUID_PAIR(6de9a68a-a928-4522-bf57-655ae3866456, IVMRSurfaceAllocatorEx9);
        GUID_PAIR(dca3f5df-bb3a-4d03-bd81-84614bfbfa0c, IVMRSurfaceAllocatorNotify9);
        GUID_PAIR(8f537d09-f85e-4414-b23b-502e54c79927, IVMRWindowlessControl9);
        GUID_PAIR(1a777eaa-47c8-4930-b2c9-8fee1c1b0f3b, IVMRMixerControl9);
        GUID_PAIR(ced175e5-1935-4820-81bd-ff6ad00c9108, IVMRMixerBitmap9);
        GUID_PAIR(dfc581a1-6e1f-4c3a-8d0a-5e9792ea2afc, IVMRSurface9);
        GUID_PAIR(45c15cab-6e22-420a-8043-ae1f0ac02c7d, IVMRImagePresenterConfig9);
        GUID_PAIR(d0cfe38b-93e7-4772-8957-0400c49a4485, IVMRVideoStreamControl9);
        GUID_PAIR(5a804648-4f66-4867-9c43-4f5c822cf1b8, IVMRFilterConfig9);
        GUID_PAIR(00d96c29-bbde-4efc-9901-bb5036392146, IVMRAspectRatioControl9);
        GUID_PAIR(46c2e457-8ba0-4eef-b80b-0680f0978749, IVMRMonitorConfig9);
        GUID_PAIR(a215fb8d-13c2-4f7f-993c-003d6271a459, IVMRDeinterlaceControl9);
        GUID_PAIR(4a5c89eb-df51-4654-ac2a-e48e02bbabf6, IVMRImageCompositor9);

        // media format
        GUID_PAIR(05589F80-C356-11CE-BF01-00AA0055595A, Format_VideoInfo);
        GUID_PAIR(F72A76A0-EB0A-11D0-ACE4-0000C0CC16BA, FORMAT_VideoInfo2);
        GUID_PAIR(E06D80E3-DB46-11CF-B4D1-00805F6CBBEA, FORMAT_MPEG2_VIDEO);
        GUID_PAIR(00000001-0000-0010-8000-00aa00389b71, KSDATAFORMAT_SUBTYPE_PCM);
        GUID_PAIR(00000003-0000-0010-8000-00aa00389b71, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
        GUID_PAIR(00000000-0000-0010-8000-00aa00389b71, KSDATAFORMAT_SUBTYPE_WAVEFORMATEX);
        GUID_PAIR(73646976-0000-0010-8000-00AA00389B71, MEDIATYPE_Video);
        GUID_PAIR(73647561-0000-0010-8000-00AA00389B71, MEDIATYPE_Audio);
        GUID_PAIR(73747874-0000-0010-8000-00AA00389B71, MEDIATYPE_Text);
        GUID_PAIR(7364696D-0000-0010-8000-00AA00389B71, MEDIATYPE_Midi);
        GUID_PAIR(e436eb83-524f-11ce-9f53-0020af0ba770, MEDIATYPE_Stream);
        GUID_PAIR(73766169-0000-0010-8000-00AA00389B71, MEDIATYPE_Interleaved);
        GUID_PAIR(656c6966-0000-0010-8000-00AA00389B71, MEDIATYPE_File);
        GUID_PAIR(73636d64-0000-0010-8000-00AA00389B71, MEDIATYPE_ScriptCommand);
        GUID_PAIR(670AEA80-3A82-11d0-B79B-00AA003767A7, MEDIATYPE_AUXLine21Data);
        GUID_PAIR(11264ACB-37DE-4eba-8C35-7F04A1A68332, MEDIATYPE_AUXTeletextPage);
        GUID_PAIR(AEB312E9-3357-43ca-B701-97EC198E2B62, MEDIATYPE_CC_CONTAINER);
        GUID_PAIR(FB77E152-53B2-499c-B46B-509FC33EDFD7, MEDIATYPE_DTVCCData);
        GUID_PAIR(B88B8A89-B049-4C80-ADCF-5898985E22C1, MEDIATYPE_MSTVCaption);
        GUID_PAIR(F72A76E1-EB0A-11D0-ACE4-0000C0CC16BA, MEDIATYPE_VBI);
        GUID_PAIR(34FFCBC3-D5B3-4171-9002-D4C60301697F, DVB_SUBTITLES);
        GUID_PAIR(059DD67D-2E55-4d41-8D1B-01F5E4F50607, ISDB_CAPTIONS);
        GUID_PAIR(36dc6d28-f1a6-4216-9048-9cfcefeb5eba, ISDB_SUPERIMPOSE);
        GUID_PAIR(0482DEE3-7817-11cf-8a03-00aa006ecb65, MEDIATYPE_Timecode);
        GUID_PAIR(74726c6d-0000-0010-8000-00AA00389B71, MEDIATYPE_LMRT);
        GUID_PAIR(74726c6d-0000-0010-8000-00AA00389B71, MEDIATYPE_URL_STREAM);

        GUID_PAIR(e436eb8e-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_None);
        GUID_PAIR(e436eb78-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB1);
        GUID_PAIR(e436eb79-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB4);
        GUID_PAIR(e436eb7a-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB8);
        GUID_PAIR(e436eb7b-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB565);
        GUID_PAIR(e436eb7c-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB555);
        GUID_PAIR(e436eb7d-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB24);
        GUID_PAIR(e436eb7e-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_RGB32);
        GUID_PAIR(297C55AF-E209-4cb3-B757-C76D6B9C88A8, MEDIASUBTYPE_ARGB1555);
        GUID_PAIR(6E6415E6-5C24-425f-93CD-80102B3D1CCA, MEDIASUBTYPE_ARGB4444);
        GUID_PAIR(773c9ac0-3274-11d0-B724-00aa006c1A01, MEDIASUBTYPE_ARGB32);
        GUID_PAIR(2f8bb76d-b644-4550-acf3-d30caa65d5c5, MEDIASUBTYPE_A2R10G10B10);
        GUID_PAIR(576f7893-bdf6-48c4-875f-ae7b81834567, MEDIASUBTYPE_A2B10G10R10);
        GUID_PAIR(e436eb7f-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_Overlay);
        GUID_PAIR(e436eb80-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEGPacket);
        GUID_PAIR(e436eb81-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEG1Payload);
        GUID_PAIR(00000050-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_MPEG1AudioPayload);
        GUID_PAIR(e436eb82-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEG1SystemStream);
        GUID_PAIR(e436eb84-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEG1System);
        GUID_PAIR(e436eb85-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEG1VideoCD);
        GUID_PAIR(e436eb86-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEG1Video);
        GUID_PAIR(e436eb87-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_MPEG1Audio);
        GUID_PAIR(e436eb88-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_Avi);
        GUID_PAIR(3DB80F90-9412-11d1-ADED-0000F8754B99, MEDIASUBTYPE_Asf);
        GUID_PAIR(e436eb89-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_QTMovie);
        GUID_PAIR(e436eb8a-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_PCMAudio_Obsolete);
        GUID_PAIR(e436eb8b-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_WAVE);
        GUID_PAIR(e436eb8c-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_AU);
        GUID_PAIR(e436eb8d-524f-11ce-9f53-0020af0ba770, MEDIASUBTYPE_AIFF);
        GUID_PAIR(6E8D4A22-310C-11d0-B79A-00AA003767A7, MEDIASUBTYPE_Line21_BytePair);
        GUID_PAIR(6E8D4A23-310C-11d0-B79A-00AA003767A7, MEDIASUBTYPE_Line21_GOPPacket);
        GUID_PAIR(6E8D4A24-310C-11d0-B79A-00AA003767A7, MEDIASUBTYPE_Line21_VBIRawData);
        GUID_PAIR(0AF414BC-4ED2-445e-9839-8F095568AB3C, MEDIASUBTYPE_708_608Data);
        GUID_PAIR(F52ADDAA-36F0-43F5-95EA-6D866484262A, MEDIASUBTYPE_DtvCcData);
        GUID_PAIR(7EA626DB-54DA-437b-BE9F-F73073ADFA3C, MEDIASUBTYPE_CC_CONTAINER);
        GUID_PAIR(F72A76E3-EB0A-11D0-ACE4-0000C0CC16BA, MEDIASUBTYPE_TELETEXT);
        GUID_PAIR(663DA43C-03E8-4e9a-9CD5-BF11ED0DEF76, MEDIASUBTYPE_VBI);
        GUID_PAIR(2791D576-8E7A-466F-9E90-5D3F3083738B, MEDIASUBTYPE_WSS);
        GUID_PAIR(01CA73E3-DCE6-4575-AFE1-2BF1C902CAF3, MEDIASUBTYPE_XDS);
        GUID_PAIR(A1B3F620-9792-4d8d-81A4-86AF25772090, MEDIASUBTYPE_VPS);
        GUID_PAIR(00000009-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_DRM_Audio);
        GUID_PAIR(00000003-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_IEEE_FLOAT);
        GUID_PAIR(00000092-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_DOLBY_AC3_SPDIF);
        GUID_PAIR(00000240-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_RAW_SPORT);
        GUID_PAIR(00000241-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_SPDIF_TAG_241h);
        GUID_PAIR(00000001-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_PCM);

        GUID_PAIR(4F303234-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_420O);
        GUID_PAIR(32335237-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_RGB32_D3D_DX7_RT);
        GUID_PAIR(36315237-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_RGB16_D3D_DX7_RT);
        GUID_PAIR(38384137-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_ARGB32_D3D_DX7_RT);
        GUID_PAIR(34344137-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_ARGB4444_D3D_DX7_RT);
        GUID_PAIR(35314137-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_ARGB1555_D3D_DX7_RT);
        GUID_PAIR(32335239-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_RGB32_D3D_DX9_RT);
        GUID_PAIR(36315239-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_RGB16_D3D_DX9_RT);
        GUID_PAIR(38384139-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_ARGB32_D3D_DX9_RT);
        GUID_PAIR(34344139-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_ARGB4444_D3D_DX9_RT);
        GUID_PAIR(35314139-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_ARGB1555_D3D_DX9_RT);
        GUID_PAIR(31435641-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_AVC1);
        GUID_PAIR(56555941-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_AYUV);
        GUID_PAIR(34344941-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_AI44);
        GUID_PAIR(4C504C43-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_CLPL);
        GUID_PAIR(524a4c43-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_CLJR);
        GUID_PAIR(414c5043-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_CPLA);
        GUID_PAIR(43434643-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_CFCC);
        GUID_PAIR(53435644-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_DVCS);
        GUID_PAIR(44535644-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_DVSD);
        GUID_PAIR(34363248-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_H264);
        GUID_PAIR(39304649-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IF09);
        GUID_PAIR(56555949-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IYUV);
        GUID_PAIR(47504A49-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IJPG);
        GUID_PAIR(31434D49-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IMC1);
        GUID_PAIR(32434d49-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IMC2);
        GUID_PAIR(33434d49-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IMC3);
        GUID_PAIR(34434d49-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IMC4);
        GUID_PAIR(34344149-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_IA44);
        GUID_PAIR(47504A4D-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_MJPG);
        GUID_PAIR(4656444D-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_MDVF);
        GUID_PAIR(3231564E-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_NV12);
        GUID_PAIR(3131564E-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_NV11);
        GUID_PAIR(3432564E-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_NV24);
        GUID_PAIR(38303250-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_P208);
        GUID_PAIR(30313050-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_P010);
        GUID_PAIR(36313050-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_P016);
        GUID_PAIR(30313250-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_P210);
        GUID_PAIR(36313250-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_P216);
        GUID_PAIR(38303450-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_P408);
        GUID_PAIR(6D756C50-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_Plum);
        GUID_PAIR(30424752-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_RGB0);
        GUID_PAIR(30343353-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_S340);
        GUID_PAIR(32343353-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_S342);
        GUID_PAIR(4A4D5654-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_TVMJ);
        GUID_PAIR(59565955-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_UYVY);
        GUID_PAIR(454B4157-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_WAKE);
        GUID_PAIR(56595559-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_YUYV);
        GUID_PAIR(39555659-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_YVU9);
        GUID_PAIR(30313459-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_Y410);
        GUID_PAIR(31313459-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_Y411);
        GUID_PAIR(50313459-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_Y41P);
        GUID_PAIR(32595559-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_YUY2);
        GUID_PAIR(55595659-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_YVYU);
        GUID_PAIR(31313259-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_Y211);
        GUID_PAIR(36313459-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_Y416);
        GUID_PAIR(32315659-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_YV12);
        GUID_PAIR(34325659-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_YV24);
        GUID_PAIR(64737664-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_dvsd);
        GUID_PAIR(64687664-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_dvhd);
        GUID_PAIR(6c737664-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_dvsl);
        GUID_PAIR(35327664-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_dv25);
        GUID_PAIR(30357664-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_dv50);
        GUID_PAIR(31687664-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_dvh1);
        GUID_PAIR(6765706a-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_jpeg);
        GUID_PAIR(617a7072-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_rpza);
        GUID_PAIR(20656c72-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_rle);
        GUID_PAIR(20636d73-0000-0010-8000-00AA00389B71, MEDIASUBTYPE_smc);
        GUID_PAIR(30313476-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_v410);
        GUID_PAIR(30313276-0000-0010-8000-00aa00389b71, MEDIASUBTYPE_v210);

        // render
        GUID_PAIR(c1f400a4-3f08-11d3-9f0b-006008039e37, NullRenderer);

        // webview2
        GUID_PAIR(9f760f8a-fb79-42be-9990-7b56900fa9c7, ICoreWebView2AcceleratorKeyPressedEventArgs);
        GUID_PAIR(b29c7e28-fa79-41a8-8e44-65811c76dcb2, ICoreWebView2AcceleratorKeyPressedEventHandler);
        GUID_PAIR(4d00c0d1-9434-4eb6-8078-8697a560334f, ICoreWebView2Controller);
        GUID_PAIR(b52d71d6-c4df-4543-a90c-64a3e60f38cb, ICoreWebView2ZoomFactorChangedEventHandler);
        GUID_PAIR(69035451-6dc7-4cb8-9bce-b2bd70ad289f, ICoreWebView2MoveFocusRequestedEventHandler);
        GUID_PAIR(2d6aa13b-3839-4a15-92fc-d88b3c0d9c9d, ICoreWebView2MoveFocusRequestedEventArgs);
        GUID_PAIR(05ea24bd-6452-4926-9014-4b82b498135d, ICoreWebView2FocusChangedEventHandler);
        GUID_PAIR(76eceacb-0462-4d94-ac83-423a6793775e, ICoreWebView2);
        GUID_PAIR(e562e4f0-d7fa-43ac-8d71-c05150499f00, ICoreWebView2Settings);
        GUID_PAIR(9adbe429-f36d-432b-9ddc-f8881fbd76e3, ICoreWebView2NavigationStartingEventHandler);
        GUID_PAIR(5b495469-e119-438a-9b18-7604f25f2e49, ICoreWebView2NavigationStartingEventArgs);
        GUID_PAIR(e86cac0e-5523-465c-b536-8fb9fc8c8c60, ICoreWebView2HttpRequestHeaders);
        GUID_PAIR(0702fc30-f43b-47bb-ab52-a42cb552ad9f, ICoreWebView2HttpHeadersCollectionIterator);
        GUID_PAIR(364471e7-f2be-4910-bdba-d72077d51c4b, ICoreWebView2ContentLoadingEventHandler);
        GUID_PAIR(0c8a1275-9b6b-4901-87ad-70df25bafa6e, ICoreWebView2ContentLoadingEventArgs);
        GUID_PAIR(3c067f9f-5388-4772-8b48-79f7ef1ab37c, ICoreWebView2SourceChangedEventHandler);
        GUID_PAIR(31e0e545-1dba-4266-8914-f63848a1f7d7, ICoreWebView2SourceChangedEventArgs);
        GUID_PAIR(c79a420c-efd9-4058-9295-3e8b4bcab645, ICoreWebView2HistoryChangedEventHandler);
        GUID_PAIR(d33a35bf-1c49-4f98-93ab-006e0533fe1c, ICoreWebView2NavigationCompletedEventHandler);
        GUID_PAIR(30d68b7d-20d9-4752-a9ca-ec8448fbb5c1, ICoreWebView2NavigationCompletedEventArgs);
        GUID_PAIR(ef381bf9-afa8-4e37-91c4-8ac48524bdfb, ICoreWebView2ScriptDialogOpeningEventHandler);
        GUID_PAIR(7390bb70-abe0-4843-9529-f143b31b03d6, ICoreWebView2ScriptDialogOpeningEventArgs);
        GUID_PAIR(c10e7f7b-b585-46f0-a623-8befbf3e4ee0, ICoreWebView2Deferral);
        GUID_PAIR(15e1c6a3-c72a-4df3-91d7-d097fbec6bfd, ICoreWebView2PermissionRequestedEventHandler);
        GUID_PAIR(973ae2ef-ff18-4894-8fb2-3c758f046810, ICoreWebView2PermissionRequestedEventArgs);
        GUID_PAIR(79e0aea4-990b-42d9-aa1d-0fcc2e5bc7f1, ICoreWebView2ProcessFailedEventHandler);
        GUID_PAIR(8155a9a4-1474-4a86-8cae-151b0fa6b8ca, ICoreWebView2ProcessFailedEventArgs);
        GUID_PAIR(b99369f3-9b11-47b5-bc6f-8e7895fcea17, ICoreWebView2AddScriptToExecuteOnDocumentCreatedCompletedHandler);
        GUID_PAIR(49511172-cc67-4bca-9923-137112f4c4cc, ICoreWebView2ExecuteScriptCompletedHandler);
        GUID_PAIR(697e05e9-3d8f-45fa-96f4-8ffe1ededaf5, ICoreWebView2CapturePreviewCompletedHandler);
        GUID_PAIR(57213f19-00e6-49fa-8e07-898ea01ecbd2, ICoreWebView2WebMessageReceivedEventHandler);
        GUID_PAIR(0f99a40c-e962-4207-9e92-e3d542eff849, ICoreWebView2WebMessageReceivedEventArgs);
        GUID_PAIR(5c4889f0-5ef6-4c5a-952c-d8f1b92d0574, ICoreWebView2CallDevToolsProtocolMethodCompletedHandler);
        GUID_PAIR(b32ca51a-8371-45e9-9317-af021d080367, ICoreWebView2DevToolsProtocolEventReceiver);
        GUID_PAIR(e2fda4be-5456-406c-a261-3d452138362c, ICoreWebView2DevToolsProtocolEventReceivedEventHandler);
        GUID_PAIR(653c2959-bb3a-4377-8632-b58ada4e66c4, ICoreWebView2DevToolsProtocolEventReceivedEventArgs);
        GUID_PAIR(d4c185fe-c81c-4989-97af-2d3fa7ab5651, ICoreWebView2NewWindowRequestedEventHandler);
        GUID_PAIR(34acb11c-fc37-4418-9132-f9c21d1eafb9, ICoreWebView2NewWindowRequestedEventArgs);
        GUID_PAIR(5eaf559f-b46e-4397-8860-e422f287ff1e, ICoreWebView2WindowFeatures);
        GUID_PAIR(f5f2b923-953e-4042-9f95-f3a118e1afd4, ICoreWebView2DocumentTitleChangedEventHandler);
        GUID_PAIR(e45d98b1-afef-45be-8baf-6c7728867f73, ICoreWebView2ContainsFullScreenElementChangedEventHandler);
        GUID_PAIR(ab00b74c-15f1-4646-80e8-e76341d25d71, ICoreWebView2WebResourceRequestedEventHandler);
        GUID_PAIR(453e667f-12c7-49d4-be6d-ddbe7956f57a, ICoreWebView2WebResourceRequestedEventArgs);
        GUID_PAIR(97055cd4-512c-4264-8b5f-e3f446cea6a5, ICoreWebView2WebResourceRequest);
        GUID_PAIR(aafcc94f-fa27-48fd-97df-830ef75aaec9, ICoreWebView2WebResourceResponse);
        GUID_PAIR(03c5ff5a-9b45-4a88-881c-89a9f328619c, ICoreWebView2HttpResponseHeaders);
        GUID_PAIR(5c19e9e0-092f-486b-affa-ca8231913039, ICoreWebView2WindowCloseRequestedEventHandler);
        GUID_PAIR(6c4819f3-c9b7-4260-8127-c9f5bde7f68c, ICoreWebView2CreateCoreWebView2ControllerCompletedHandler);
        GUID_PAIR(4e8a3389-c9d8-4bd2-b6b5-124fee6cc14d, ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler);
        GUID_PAIR(b96d755e-0319-4e92-a296-23436f46a1fc, ICoreWebView2Environment);
        GUID_PAIR(f9a2976e-d34e-44fc-adee-81b6b57ca914, ICoreWebView2NewBrowserVersionAvailableEventHandler);
        GUID_PAIR(2fde08a8-1e9a-4766-8c05-95a9ceb9d1c5, ICoreWebView2EnvironmentOptions);

#undef GUID_PAIR
    }
};

inline static TM::CString DbgFormatGuid(const GUID& guid)
{
    static _CGUIDMap_for_format _map;
    _CGUIDMap_for_format::recorder rec(_map);
    rec.find(guid);
    if (rec)
        return rec.get_data();

    TM::CString str;
    str.Format(TM_T("%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"),
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2],
        guid.Data4[3], guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    return std::move(str);
}


struct _DebugGXFormat2Str
{
    mutable TMUInt64 c;
    DbgAssertS(gxMakeFourCC('Y', 'U', 'Y', '2') == gxMakeFourCC1('YUY2'));

    const char* done(TMUInt32 fmt) const
    {
        switch (fmt)
        {
        case eGXFormatPAL2:         return "eGXFormatPAL2";
        case eGXFormatPAL4:         return "eGXFormatPAL4";
        case eGXFormatPAL8:         return "eGXFormatPAL8";
        case eGXFormat16:           return "eGXFormat16";
        case eGXFormat24:           return "eGXFormat24";
        case eGXFormat32A:          return "eGXFormat32A";
        case eGXFormat32X:          return "eGXFormat32X";

        case eGXFormatA8B8G8R8:     return "eGXFormatA8B8G8R8";
        case eGXFormatX8B8G8R8:     return "eGXFormatX8B8G8R8";

        case eGXFormatR8G8B8:       return "eGXFormatR8G8B8";
        case eGXFormatA8R8G8B8:     return "eGXFormatA8R8G8B8";
        case eGXFormatX8R8G8B8:     return "eGXFormatX8R8G8B8";
        case eGXFormatR5G6B5:       return "eGXFormatR5G6B5";
        case eGXFormatA1R5G5B5:     return "eGXFormatA1R5G5B5";
        case eGXFormatA4R4G4B4:     return "eGXFormatA4R4G4B4";
        case eGXFormatR3G3B2:       return "eGXFormatR3G3B2";
        case eGXFormatA8:           return "eGXFormatA8";
        case eGXFormatA8R3G3B2:     return "eGXFormatA8R3G3B2";
        case eGXFormatX4R4G4B4:     return "eGXFormatX4R4G4B4";
        case eGXFormatA2B10G10R10:  return "eGXFormatA2B10G10R10";
        case eGXFormatG16R16:       return "eGXFormatG16R16";
        case eGXFormatA2R10G10B10:  return "eGXFormatA2R10G10B10";
        case eGXFormatA16B16G16R16: return "eGXFormatA16B16G16R16";

        case eGXFormatA8P8:         return "eGXFormatA8P8";
        case eGXFormatP8:           return "eGXFormatP8";
        case eGXFormatL8:           return "eGXFormatL8";
        case eGXFormatA8L8:         return "eGXFormatA8L8";
        case eGXFormatA4L4:         return "eGXFormatA4L4";
        case eGXFormatV8U8:         return "eGXFormatV8U8";
        case eGXFormatL6V5U5:       return "eGXFormatL6V5U5";
        case eGXFormatX8L8V8U8:     return "eGXFormatX8L8V8U8";
        case eGXFormatQ8W8V8U8:     return "eGXFormatQ8W8V8U8";
        case eGXFormatV16U16:       return "eGXFormatV16U16";
        case eGXFormatA2W10V10U10:  return "eGXFormatA2W10V10U10";

        case eGXFormatDXVA_MP2A:    return "DXVA_MP2A";
        case eGXFormatDXVA_MP2C:    return "DXVA_MP2C";
        case eGXFormatDXVA_WMVA:    return "DXVA_WMVA";
        case eGXFormatDXVA_WMVB:    return "DXVA_WMVB";
        case eGXFormatDXVA_WMVC:    return "DXVA_WMVC";
        case eGXFormatDXVA_VC1A:    return "DXVA_VC1A";
        case eGXFormatDXVA_VC1B:    return "DXVA_VC1B";
        case eGXFormatDXVA_VC1C:    return "DXVA_VC1C";
        }

        if (fmt > 200)
            c = fmt;
        else
            StrPrintf((char*)&c, sizeof(TMUInt64), "%d", fmt);

        return (const char*)&c;
    }
};


inline static TM::CString DbgFormatGXFormat(TMUInt32 x)
{
    return _DebugGXFormat2Str().done((x));
}

#define GX_DEBUG_FORMAT2STR(x) _DebugGXFormat2Str().done((x))



#endif // TMFORMATGUID_H
