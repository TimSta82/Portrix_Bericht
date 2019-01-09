// Feedback
final ViewGroup lyFeedback = parentView.findViewById(R.id.lyFeedbackPin);
lyFeedback.setVisibility(View.VISIBLE);
TextView tvFeedback = lyFeedback.findViewById(R.id.tvFeedbackPin);
tvFeedback.setText(getString(R.string.fuel_station_detail_feedback));
lyFeedback.setOnClickListener(new View.OnClickListener() {

    @Override
    public void onClick(View v) {
        Bundle b = getArguments();
        Feedback feedbackFragment = new Feedback();
        feedbackFragment.setArguments(b);
        getSupportFragmentManager()
				.beginTransaction()
				.setCustomAnimations(
						R.animator.enter_from_right,
						R.animator.exit_to_right,
						R.animator.enter_from_right,
						R.animator.exit_to_right)
				.add(R.id.fragmentContainer, feedbackFragment, Feedback.TAG)
				.addToBackStack(null)
				.commit();
    }
});


