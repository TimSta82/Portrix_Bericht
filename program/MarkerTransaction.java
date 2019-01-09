@Override
public boolean onMarkerClick(final Marker marker) {
    // Click on a fuel station?
    FuelStation station = mMapHelper.getMarkerMap().get(marker);
    if (station == null) {
        Log.d(TAG, "No Station found");
        return false;
    }
    selectedStation = station.getIdx();
    Bundle b = new Bundle();
    b.putInt("station", selectedStation);
    FuelStationDetail fuelStationDetail = new FuelStationDetail();
    fuelStationDetail.setArguments(b);
    getSupportFragmentManager()
				.beginTransaction()
				.setCustomAnimations(
						R.animator.enter_from_right,
						R.animator.exit_to_right,
						R.animator.enter_from_right,
						R.animator.exit_to_right)
				.add(R.id.fragmentContainer, fuelStationDetail, FuelStationDetail.TAG)
				.addToBackStack(null)
				.commit();

    return true;
}

