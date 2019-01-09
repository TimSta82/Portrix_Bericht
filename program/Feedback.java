
@Override
public void onCreate(@Nullable Bundle savedInstanceState) {
	super.onCreate(savedInstanceState);
	//mContext = (FuelStationsMap) getActivity(); // initialisierung für veraltete Version
	mContext = FuelStationsMap.getMap();
	mContext.addTrackingScreen(getString(R.string.trackingname_view_detail_open));
	startTimer();
}

