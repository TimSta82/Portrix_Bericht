@SuppressLint("SetJavaScriptEnabled")
private void initView() {

	// GoogleAnalytics opt out
	class GoogleAnalytics {
		@JavascriptInterface
		public void postMessage(String action) {
			try {
				JSONObject json = new JSONObject(action);				
				String a = json.getString("action");
				Log.d(TAG, "Message: " + a);
				if ("optOut".equals(a)) {
					activity.setTrackerOptOut(true);	// disable GoogleAnalytics 
													
				}
			} catch (JSONException e) {
				Log.e(TAG, e.getLocalizedMessage());
			}

		}
	}

	addJavascriptInterface(new GoogleAnalytics(), "googleAnalytics");
}
