import sys
import py_models
from utilities import load_file_info


def train(
	path_to_train_data
	, path_to_test_data
	, path_to_save_nn_dump
	, path_to_save_training_score_info
	, model_name='NeuralNetModel'
):
    # load data
    X_train, y_train = load_file_info(path_to_train_data)
    X_test, y_test = load_file_info(path_to_test_data)

    # create model
    if model_name == 'NeuralNetModel':
    	model = py_models.NeuralNetModel(path_to_save_nn_dump, input_dim=len(X_train[0]), number_of_classes=len(set(list(y_train) + list(y_test))))
    elif model_name == 'RandomForestModel':
    	model = py_models.RandomForestModel(path_to_save_nn_dump, n_estimators=300)
    else:
    	raise Exception('py_run_auth::train(...). Invalid "model_name" parameter ({0})'.format(model_name))

    # fit and save dump
    model.train_and_save(X_train, y_train)

    # return test score if we have test data
    if len(y_test) != 0:
    	test_result = model.test(X_test, y_test)
    	print test_result


def predict(
	testing_record_features_path
	, path_to_saved_nn_dump
	, path_to_store_result
	, model_name='NeuralNetModel'
):
    if model_name == 'NeuralNetModel':
    	model = py_models.NeuralNetModel(path_to_saved_nn_dump)
    elif model_name == 'RandomForestModel':
    	model = py_models.RandomForestModel(path_to_saved_nn_dump)
    else:
    	raise Exception('py_run_auth::train(...). Invalid "model_name" parameter ({0})'.format(model_name))

    model.predict_one_wav(testing_record_features_path, path_to_store_result)


if __name__ == '__main__':
    if sys.argv[1] == 'fit':
        train(*sys.argv[2:])
    elif sys.argv[1] == 'predict':
        predict(*sys.argv[2:])
