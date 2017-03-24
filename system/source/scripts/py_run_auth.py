import sys
import py_models
from utilities import load_file_info


def train(
	path_to_train_data
	, path_to_test_data
	, path_to_save_nn_dump
	, path_to_save_training_score_info
	, model_name='NN'
    , use_two_step_classification='0'
    , path_to_save_secondary_model=None
    , path_to_secondary_model_train=None
):
    # check parameters
    use_two_step_classification = True if use_two_step_classification == '1' else False

    if use_two_step_classification and (not path_to_save_secondary_model or not path_to_secondary_model_train):
        raise Exception('py_run_auth::train(...). Parameter "use_two_step_classification" = True, but other args for that mode were not specified')

    # load data
    X_train, y_train = load_file_info(path_to_train_data)
    X_test, y_test = load_file_info(path_to_test_data)

    # create specified model
    if model_name == 'NN':
    	model = py_models.NeuralNetModel(path_to_save_nn_dump, input_dim=len(X_train[0]), number_of_classes=len(set(list(y_train) + list(y_test))))
    elif model_name == 'RF':
    	model = py_models.RandomForestModel(path_to_save_nn_dump, n_estimators=300)
    else:
    	raise Exception('py_run_auth::train(...). Invalid "model_name" parameter ({0})'.format(model_name))

    # fit and save dump
    model.train_and_save(X_train, y_train)

    # fit and save secondary model if we want to
    if use_two_step_classification:
        X_train_secondary, y_train_secondary = load_file_info(path_to_secondary_model_train)

        # secondary_model = py_models.SecondaryModel(path_to_save_secondary_model)
        # secondary_model.train_and_save(X_train_secondary, y_train_secondary)

    # return test score if we have test data
    if len(y_test) != 0:
    	test_result = model.test(X_test, y_test)
        print 'Test result: {0}'.format(test_result)


def predict(
	testing_record_features_path
	, path_to_saved_nn_dump
	, path_to_store_result
	, model_name='NN'
    , use_two_step_classification='0'
    , path_to_saved_secondary_model=None
    , path_to_testing_features_for_secondary_model=None
):
    # check parameters
    use_two_step_classification = True if use_two_step_classification == '1' else False

    if use_two_step_classification and (not path_to_saved_secondary_model or not path_to_testing_features_for_secondary_model):
        raise Exception('py_run_auth::train(...). Parameter "use_two_step_classification" = True, but other args for that mode were not specified')
    
    # create specified model
    if model_name == 'NN':
    	model = py_models.NeuralNetModel(path_to_saved_nn_dump)
    elif model_name == 'RF':
    	model = py_models.RandomForestModel(path_to_saved_nn_dump)
    else:
    	raise Exception('py_run_auth::train(...). Invalid "model_name" parameter ({0})'.format(model_name))

    # first step classification
    classification_results = model.predict_one_wav(testing_record_features_path, predict_mode='probabilities')
    result_class = None

    # check and run (if we need) second step of classification
    if use_two_step_classification:
        # classification_results = sorted(classification_results, reverse=True)
        # if classification_results[0] / classification_results[1] <= 100:
        #     result_class = 1
        #     secondary_model = py_models.SecondaryModel(path_to_saved_secondary_model)
        #     result_class = secondary_model.predict_one_wav(path_to_testing_features_for_secondary_model, predict_mode='class-id')
        result_class = np.argmax(classification_results) + 1
    else:
        # if we do not need secondary classification - use just argmax of all probabilities
        result_class = np.argmax(classification_results) + 1

    assert(result_class is not None)
    with open(path_to_store_result, 'w') as outf:
        outf.write(str(result_class))


if __name__ == '__main__':
    if sys.argv[1] == 'fit':
        train(*sys.argv[2:])
    elif sys.argv[1] == 'predict':
        predict(*sys.argv[2:])
